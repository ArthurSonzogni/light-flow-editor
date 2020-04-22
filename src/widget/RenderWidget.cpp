// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "widget/RenderWidget.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <smk/Shape.hpp>

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
    : smkflow::Widget(delegate), framebuffer_(size_, size_) {
  square_ = smk::Shape::Square();
  square_.SetPosition(-0.5f, -0.5f);
  square_.SetScale(1.0f, 1.f);

  vertex_shader_ = smk::Shader::FromString(R"(
      layout(location = 0) in vec2 space_position;
      layout(location = 1) in vec2 texture_position;
      uniform mat4 projection;
      uniform mat4 view;
      out vec2 screen_position;
      void main() {
        gl_Position = projection * view * vec4(space_position, 0.0, 1.0);
        screen_position = gl_Position.xy;
      }
    )",
                                           GL_VERTEX_SHADER);
  Build(R"(
      in vec2 screen_position;
      out vec4 out_color;

      void main() {
        out_color = vec4(0.f, 0.f, 0.f, 1.f);
      }
    )");
}

glm::vec2 RenderWidget::ComputeDimensions() {
  return {256.f, 256.f};
}

void RenderWidget::Draw(smk::RenderTarget* target) {
  framebuffer_.Clear({0.2, 0.2, 0.2, 1.0});
  framebuffer_.SetShaderProgram(&shader_program_);

  auto view = smk::View();
  view.SetCenter(0.f, 0.f);
  view.SetSize(1.f, 1.f);
  framebuffer_.SetView(view);

  shader_program_.Use();
  shader_program_.SetUniform("time", g_time);
  shader_program_.SetUniform(
      "square_rotation",
      glm::rotate(glm::mat4(1.0), g_time * 0.04f, glm::vec3(0.f, 1.f, 0.f)));

  glm::mat4 sphere_rotation = glm::mat4(1.f);
  sphere_rotation =
      glm::rotate(sphere_rotation, -g_time * 0.4f, glm::vec3(0.f, 1.f, 0.f));
  sphere_rotation = glm::translate(sphere_rotation, glm::vec3(0.f, +3.f, -3.f));
  sphere_rotation =
      glm::rotate(sphere_rotation, float(M_PI / 4.f), glm::vec3(1.f, 0.f, 0.f));
  shader_program_.SetUniform("sphere_rotation", sphere_rotation);
  framebuffer_.Draw(square_);

  sprite_.SetPosition(Position());
  target->Draw(sprite_);
}

void RenderWidget::Build(std::string new_code) {
  if (new_code.size() == 0)
    return;
  if (new_code == code_)
    return;
  code_ = std::move(new_code);

  fragment_shader_ = smk::Shader::FromString(code_, GL_FRAGMENT_SHADER);

  shader_program_ = smk::ShaderProgram();
  shader_program_.AddShader(vertex_shader_);
  shader_program_.AddShader(fragment_shader_);
  shader_program_.Link();

  sprite_ = smk::Sprite(framebuffer_);
  sprite_.SetScale(256.f / size_, 256.f / size_);
}
