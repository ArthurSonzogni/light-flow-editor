// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <fmt/core.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <smk/Window.hpp>
#include <smkflow/Constants.hpp>
#include <smkflow/Elements.hpp>
#include <smkflow/Model.hpp>
#include <smkflow/widget/Box.hpp>
#include <smkflow/widget/Input.hpp>
#include <smkflow/widget/Label.hpp>
#include <smkflow/widget/Menu.hpp>
#include <smkflow/widget/Slider.hpp>
#include <smkflow/widget/Widget.hpp>
#include <sstream>

#include "widget/RenderWidget.hpp"

float g_time;
enum Node {
  Screen,

  Number,
  Time,

  NewVec3,
  Color,

  Union,
  SmoothedUnion,
  Intersection,
  Complement,
  Difference,

  Repeat,

  Sphere,
  Torus,
  Cube,

  Translate,
  Scale,
};

auto type_float = glm::vec4{0.7f, 0.7f, 1.f, 1.f};
auto type_vec3 = glm::vec4{0.7f, 1.f, 1.f, 1.f};
auto type_sdf = glm::vec4{1.0f, 0.7f, 0.7f, 1.f};

auto model_type_fusion = glm::vec4{0.5, 0.4, 0.4, 1.0f};
auto model_type_screen = glm::vec4{0.5, 0.5, 0.5, 1.0f};
auto model_type_primitive = glm::vec4{0.4, 0.5, 0.4, 1.0f};
auto model_type_transformation = glm::vec4{0.4, 0.4, 0.5, 1.0f};

using smkflow::CreateNode;
using smkflow::Menu;
using smkflow::MenuEntry;

class Context {
 public:
  std::string Identifier() {
    std::string out;
    int v = ++value_;
    while (v) {
      out += ('a' + v % 26);
      v /= 26;
    }
    return out;
  }

  void RegisterFunction(const char* f) { function_set_.insert(f); }

  std::string RegisteredFunction() {
    std::string out;
    for (auto& it : function_set_)
      out += it;
    return out;
  }

  void MarkNotImplemented() { implemented_ = false; }
  bool IsImplemented() { return implemented_; }

 private:
  bool implemented_ = true;
  int value_ = 0;
  std::set<const char*> function_set_;
};

std::string BuildSDF(smkflow::Node*,
                     const std::string& in,
                     const std::string& out,
                     Context* context);

std::string BuildFloat(smkflow::Node*,
                       const std::string& out,
                       Context* context);

std::string BuildVec3(smkflow::Node* node,
                      const std::string& out,
                      Context* context);

std::string BuildUnimplemented(Context* context) {
  context->MarkNotImplemented();
  return "  // Not connected.";
}

std::string BuildUnimplemented(smkflow::Node* node, Context* context) {
  context->MarkNotImplemented();
  return fmt::format("  // Not implemented. Identifier = {}",
                     node->Identifier());
}

#include "node/color.ipp"
#include "node/complement.ipp"
#include "node/cube.ipp"
#include "node/difference.ipp"
#include "node/intersection.ipp"
#include "node/new_vec3.ipp"
#include "node/number.ipp"
#include "node/repeat.ipp"
#include "node/scale.ipp"
#include "node/screen.ipp"
#include "node/smoothed_union.ipp"
#include "node/sphere.ipp"
#include "node/time.ipp"
#include "node/torus.ipp"
#include "node/translate.ipp"
#include "node/union.ipp"

const char* header = R"(
in vec2 screen_position;
uniform vec4 color;
uniform mat4 square_rotation;
uniform mat4 sphere_rotation;
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
    for( float t=mint; t<maxt; )
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

  position = ray(position, direction);
  if (position.z > 100.f) {
    out_color = vec4(0.f, 0.f, 0.f, 1.f);

  } else {

    vec3 light_position = vec3(1.f, 3.1, -1.1);
    vec3 light_direction = normalize(light_position - position);
    vec3 surface_direction = differential(position);
    vec3 reflection_direction = -reflect(direction, surface_direction);

    float ambient_color = 0.2;
    float diffuse_color = 0.6 * max(0.f, dot(surface_direction, light_direction));
    float specular_color = 0.2 * max(0.f, pow(dot(reflection_direction, light_direction), 3.0));

    float phong = ambient_color + diffuse_color + specular_color;
    vec3 color = sdf(position).color;
    
    position += 0.01f * surface_direction;
    direction = light_direction;

    float shadow = 0.2+0.8*softshadow(position, light_direction,
        0.1f, distance(position, light_position), 5.f );

    phong *= shadow;
    color *= phong;
    out_color = vec4(color, 1.f);
  }
}
)";

std::string BuildSDF(smkflow::Node* node,
                     const std::string& in,
                     const std::string& out,
                     Context* context) {
  if (!node)
    return BuildUnimplemented(context);
  switch (node->Identifier()) {
    case Node::Color:
      return BuildColor(node, in, out, context);
    case Node::Cube:
      return BuildCube(node, in, out, context);
    case Node::Torus:
      return BuildTorus(node, in, out, context);
    case Node::Sphere:
      return BuildSphere(node, in, out, context);
    case Node::Union:
      return BuildUnion(node, in, out, context);
    case Node::SmoothedUnion:
      return BuildSmoothedUnion(node, in, out, context);
    case Node::Intersection:
      return BuildIntersection(node, in, out, context);
    case Node::Difference:
      return BuildDifference(node, in, out, context);
    case Node::Scale:
      return BuildScale(node, in, out, context);
    case Node::Translate:
      return BuildTranslate(node, in, out, context);
    case Node::Repeat:
      return BuildRepeat(node, in, out, context);
    default:
      return BuildUnimplemented(node, context);
  }
}

std::string BuildFloat(smkflow::Node* node,
                       const std::string& out,
                       Context* context) {
  if (!node)
    return BuildUnimplemented(context);
  switch (node->Identifier()) {
    case Node::Number:
      return BuildNumber(node, out, context);
    default:
      return BuildUnimplemented(node, context);
  }
}

std::string BuildVec3(smkflow::Node* node,
                      const std::string& out,
                      Context* context) {
  if (!node)
    return BuildUnimplemented(context);
  switch (node->Identifier()) {
    case Node::NewVec3:
      return BuildNewVec3(node, out, context);
    default:
      return BuildUnimplemented(node, context);
  }
}

std::string Build(smkflow::Board* board) {
  // Find sink (e.g. screen).
  for (int i = 0; i < board->NodeCount(); ++i) {
    smkflow::Node* node = board->NodeAt(i);

    if (node->Identifier() != Node::Screen)
      continue;

    Context context;
    auto out = BuildScreen(node, &context);
    if (!context.IsImplemented())
      continue;

    auto widget = RenderWidget::From(node->widget());
    widget->Build(header + out + footer);
  }
  return "";
}

auto my_board = smkflow::model::Board{
    {
        MenuEntry("screen", CreateNode(model_screen)),
        Menu("Values",
             {
                 MenuEntry("Number", CreateNode(model_number)),
                 MenuEntry("Time", CreateNode(model_time)),
                 MenuEntry("Vec3", CreateNode(model_new_vec3)),
             }),
        Menu("Shape",
             {
                 MenuEntry("Torus", CreateNode(model_torus)),
                 MenuEntry("Cube", CreateNode(model_cube)),
                 MenuEntry("Sphere", CreateNode(model_sphere)),
             }),
        Menu("Combine",
             {
                 MenuEntry("Union", CreateNode(model_union)),
                 MenuEntry("Smoothed_union", CreateNode(model_smoothed_union)),
                 MenuEntry("Complement", CreateNode(model_complement)),
                 MenuEntry("Difference", CreateNode(model_difference)),
                 MenuEntry("Intersection", CreateNode(model_intersection)),
             }),
        Menu("Transform",
             {
                 MenuEntry("Color", CreateNode(model_color)),
                 MenuEntry("Repeat", CreateNode(model_repeat)),
                 MenuEntry("Scale", CreateNode(model_scale)),
                 MenuEntry("Translate", CreateNode(model_translate)),
             }),
    },
    "../resources/arial.ttf",
};

int main() {
  // Open a new window.
  auto window = smk::Window(512, 512, "test");
  auto board = smkflow::Board::Create(my_board);

  // Instanciate some Node based on the model.
  board->Create(model_screen)->SetPosition({0, 100});
  board->Create(model_sphere)->SetPosition({100, 200});
  board->Create(model_cube)->SetPosition({0, 100});
  board->Create(model_intersection)->SetPosition({200, 0});
  board->Create(model_union)->SetPosition({400, 0});

  std::string previous_shader_code;
  window.ExecuteMainLoop([&] {
    g_time = window.time();

    window.shader_program_2d()->Use();
    window.PoolEvents();
    window.Clear(smkflow::color::background);

    board->Step(&window, &window.input());
    board->Draw(&window);

    window.Display();

    Build(board.get());
  });
  return EXIT_SUCCESS;
}
