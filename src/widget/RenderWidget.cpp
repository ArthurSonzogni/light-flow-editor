// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "widget/RenderWidget.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <smk/Input.hpp>
#include <smk/Shape.hpp>
#include <smk/RenderTarget.hpp>

float sensibility = 0.01f;

const char* header = R"(
in vec2 screen_position;
uniform vec4 color;
uniform mat4 camera;
uniform float time;
out vec4 out_color;

struct Value {
  vec3 color;
  float distance;
};

)";

const char* footer = R"(

vec3 differential(vec3 position) {
  float d = 0.001f;
  float b = sdf(position).distance;
  float fx = sdf(position + vec3(+d, +0, +0)).distance;
  float fy = sdf(position + vec3(+0, +d, +0)).distance;
  float fz = sdf(position + vec3(+0, +0, +d)).distance;
  return normalize(vec3(fx,fy, fz) - vec3(b));
}

vec3 ray(vec3 pos, vec3 direction) {
  float distance = 0.f;
  for(int i = 0; i<64; ++i) {
    distance = sdf(pos).distance;
    pos += direction * distance;
    if (distance < 0.001f)
      break;
  }

  return pos;
}

float softshadow(vec3 ro, vec3 rd, float mint, float maxt, float k ) {
    float res = 1.0;
    float ph = 1e20;
    int i = 0;
    for( float t=mint; t<maxt && i<100; ++i)
    {
        float h = sdf(ro + rd*t).distance;
        if( h<0.001 )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h * 0.1;
    }
    return res;
}

void main() {
  vec3 direction = normalize(vec3(screen_position, 1.f));
  vec3 position = vec3(0.f);

  position = (camera * vec4(position, 1.f)).xyz;
  direction = normalize(camera * vec4(direction, 0.f)).xyz;

  position = ray(position, direction);
  if (dot(position, position) > 10000.f) {
    out_color = vec4(0.f, 0.f, 0.f, 1.f);
  } else {
    vec3 light_position = vec3(3.f, 5.f, -1.f);
    vec3 light_direction = normalize(light_position - position);
    vec3 surface_direction = differential(position);
    vec3 reflection_direction = -reflect(direction, surface_direction);

    float ambient_color = 0.2;
    float diffuse_color = 0.6 * max(0.f, dot(surface_direction, light_direction));
    float specular_color = 0.2 * max(0.f, pow(dot(reflection_direction, light_direction), 3.0));

    vec3 color = sdf(position).color;
    
    direction = light_direction;

    float shadow = 0.2+0.8*softshadow(position, light_direction,
        0.1f, distance(position, light_position), 20.f );

    color *= ambient_color + shadow * (diffuse_color + specular_color);
    out_color = vec4(color, 1.f);
  }
}
)";

// static
smkflow::WidgetFactory RenderWidget::Create() {
  return [](smkflow::Widget::Delegate* delegate) {
    return std::make_unique<RenderWidget>(delegate);
  };
}

// static
RenderWidget* RenderWidget::From(smkflow::Widget* node) {
  return dynamic_cast<RenderWidget*>(node);
}

RenderWidget::RenderWidget(smkflow::Widget::Delegate* delegate)
    : smkflow::Widget(delegate) {
  camera_angle_ = {-M_PI / 4 / sensibility, (-M_PI / 8) / sensibility};
  square_ = smk::Shape::FromVertexArray(smk::VertexArray({
      {{0.f, 0.f}, {-0.5f, +0.5f}},
      {{0.f, 1.f}, {-0.5f, -0.5f}},
      {{1.f, 1.f}, {+0.5f, -0.5f}},

      {{0.f, 0.f}, {-0.5f, +0.5f}},
      {{1.f, 1.f}, {+0.5f, -0.5f}},
      {{1.f, 0.f}, {+0.5f, +0.5f}},
  }));
  square_.SetScale(256.f, 256.f);

  vertex_shader_ = smk::Shader::FromString(R"(
      layout(location = 0) in vec2 space_position;
      layout(location = 1) in vec2 texture_position;
      uniform mat4 projection;
      uniform mat4 view;
      out vec2 screen_position;
      void main() {
        gl_Position = projection * view * vec4(space_position, 0.0, 1.0);
        screen_position = texture_position.xy;
      }
    )",
                                           GL_VERTEX_SHADER);
  fragment_shader_ = smk::Shader::FromString(
      R"(
      in vec2 screen_position;
      out vec4 out_color;

      void main() {
        out_color = vec4(0.f, 0.f, 0.f, 1.f);
      }
    )",
      GL_FRAGMENT_SHADER);

  shader_program_ = smk::ShaderProgram();
  shader_program_.AddShader(vertex_shader_);
  shader_program_.AddShader(fragment_shader_);
  shader_program_.Link();
}

bool RenderWidget::Step(smk::Input* input, const glm::vec2& cursor) {
  if (cursor_captured_ && input->IsCursorReleased())
    cursor_captured_.Invalidate();

  auto position = Position();
  auto dim = dimensions();
  bool hover = cursor.x > position.x &&          //
               cursor.x < position.x + dim.x &&  //
               cursor.y > position.y &&          //
               cursor.y < position.y + dim.y;    //

  if (hover && input->IsCursorPressed()) {
    cursor_captured_ = delegate()->CaptureCursor();
    cursor_camera_drag_ = camera_angle_ - cursor;
  }

  if (cursor_captured_) {
    camera_angle_ = cursor_camera_drag_ + cursor;
    camera_angle_ *= sensibility;
    camera_angle_.y =
        std::max((float)-M_PI / 2, std::min((float)+M_PI / 2, camera_angle_.y));
    camera_angle_ /= sensibility;
  }

  return Widget::Step(input, cursor);
}

glm::vec2 RenderWidget::ComputeDimensions() {
  return {256.f, 256.f};
}

void RenderWidget::Draw(smk::RenderTarget* target) {
  target->SetShaderProgram(&shader_program_);

  shader_program_.Use();
  shader_program_.SetUniform("time", g_time);

  {
    glm::mat4 cam = glm::mat4(1.f);
    cam = glm::rotate(cam, camera_angle_.x * sensibility, {0.f, 1.f, 0.f});
    cam = glm::rotate(cam, camera_angle_.y * sensibility, {1.f, 0.f, 0.f});
    cam = glm::translate(cam, glm::vec3(0.f, +6.f, -6.f));
    cam = glm::rotate(cam, float(M_PI / 4.f), glm::vec3(1.f, 0.f, 0.f));
    shader_program_.SetUniform("camera", cam);
  }
  square_.SetPosition(Position());
  target->Draw(square_);

  target->SetShaderProgram(target->shader_program_2d());
}

void RenderWidget::Build(std::string new_code) {
  if (new_code.size() == 0)
    return;
  if (new_code == code_)
    return;

  code_ = std::move(new_code);

  fragment_shader_ = smk::Shader::FromString(header + code_ + footer, GL_FRAGMENT_SHADER);

  shader_program_ = smk::ShaderProgram();
  shader_program_.AddShader(vertex_shader_);
  shader_program_.AddShader(fragment_shader_);
  shader_program_.Link();
  return;
}
