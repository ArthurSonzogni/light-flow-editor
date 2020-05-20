// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <fmt/core.h>

#include <fstream>
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

  Slider,
  Time,

  NewVec3,
  Color,

  Union,
  Difference,
  Intersection,
  Complement,

  SmoothUnion,
  SmoothIntersection,
  SmoothDifference,

  Repeat,

  Sphere,
  Torus,
  Cube,
  Capsule,

  Translate,
  Rotate,
  Scale,

  MathSin,
  MathCos,
  MathAdd,
  MathSub,
  MathMul,
  MathDiv,
};

auto type_float = glm::vec4{0.7f, 0.7f, 1.f, 1.f};
auto type_vec3 = glm::vec4{0.7f, 1.f, 1.f, 1.f};
auto type_sdf = glm::vec4{1.0f, 0.7f, 0.7f, 1.f};

auto model_type_fusion = glm::vec4{0.5, 0.4, 0.4, 1.0f};
auto model_type_screen = glm::vec4{0.5, 0.5, 0.5, 1.0f};
auto model_type_primitive = glm::vec4{0.4, 0.5, 0.4, 1.0f};
auto model_type_transformation = glm::vec4{0.4, 0.4, 0.5, 1.0f};
auto model_type_math = glm::vec4{0.7f, 0.7f, 0.4f, 1.f};

using smkflow::CreateNode;
using smkflow::Menu;
using smkflow::MenuEntry;

class Context {
 public:
  std::string Identifier() {
    std::string out;
    int v = ++value_;
    return "v" + std::to_string(v);
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

#include "node/combine/complement.ipp"
#include "node/combine/difference.ipp"
#include "node/combine/intersection.ipp"
#include "node/combine/smooth_difference.ipp"
#include "node/combine/smooth_intersection.ipp"
#include "node/combine/smooth_union.ipp"
#include "node/combine/union.ipp"
#include "node/math/operations.ipp"
#include "node/math/trigonometry.ipp"
#include "node/new_vec3.ipp"
#include "node/primitive/capsule.ipp"
#include "node/primitive/cube.ipp"
#include "node/primitive/sphere.ipp"
#include "node/primitive/torus.ipp"
#include "node/screen.ipp"
#include "node/slider.ipp"
#include "node/time.ipp"
#include "node/transform/color.ipp"
#include "node/transform/repeat.ipp"
#include "node/transform/scale.ipp"
#include "node/transform/translate.ipp"
#include "node/transform/rotate.ipp"

std::string BuildSDF(smkflow::Node* node,
                     const std::string& in,
                     const std::string& out,
                     Context* context) {
  if (!node)
    return BuildUnimplemented(context);
  switch (node->Identifier()) {
    // -------------------------------------------------------------------------
    case Node::Cube:
      return BuildCube(node, in, out, context);
    case Node::Torus:
      return BuildTorus(node, in, out, context);
    case Node::Capsule:
      return BuildCapsule(node, in, out, context);
    case Node::Sphere:
      return BuildSphere(node, in, out, context);

    // -------------------------------------------------------------------------
    case Node::Union:
      return BuildUnion(node, in, out, context);
    case Node::Intersection:
      return BuildIntersection(node, in, out, context);
    case Node::Difference:
      return BuildDifference(node, in, out, context);

    // -------------------------------------------------------------------------
    case Node::SmoothUnion:
      return BuildSmoothUnion(node, in, out, context);
    case Node::SmoothIntersection:
      return BuildSmoothIntersection(node, in, out, context);
    case Node::SmoothDifference:
      return BuildSmoothDifference(node, in, out, context);

    // -------------------------------------------------------------------------
    case Node::Scale:
      return BuildScale(node, in, out, context);
    case Node::Translate:
      return BuildTranslate(node, in, out, context);
    case Node::Rotate:
      return BuildRotate(node, in, out, context);
    case Node::Repeat:
      return BuildRepeat(node, in, out, context);
    case Node::Color:
      return BuildColor(node, in, out, context);

    // -------------------------------------------------------------------------
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
    case Node::Slider:
      return BuildSlider(node, out, context);
    case Node::MathSin:
      return BuildMathSin(node, out, context);
    case Node::MathCos:
      return BuildMathCos(node, out, context);
    case Node::MathAdd:
      return BuildMathAdd(node, out, context);
    case Node::MathSub:
      return BuildMathSub(node, out, context);
    case Node::MathMul:
      return BuildMathMul(node, out, context);
    case Node::MathDiv:
      return BuildMathDiv(node, out, context);
    case Node::Time:
      return BuildTime(node, out, context);
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

void Build(smkflow::Board* board, int i) {
  // Find sink (e.g. screen).
  std::vector<smkflow::Node*> sinks;
  for (int i = 0; i < board->NodeCount(); ++i) {
    smkflow::Node* node = board->NodeAt(i);

    if (node->Identifier() != Node::Screen)
      continue;
    sinks.push_back(node);
  }

  if (sinks.size() == 0)
    return;

  smkflow::Node* node = sinks[i % sinks.size()];
  
  Context context;
  auto out = BuildScreen(node, &context);
  if (!context.IsImplemented())
    return;

  RenderWidget* widget = RenderWidget::From(node->widget());
  if (widget->code() != out)
    widget->Build(out);
}

auto save = [](smkflow::ActionContext* context) {
  std::ofstream("save.json") << context->board()->Serialize();
};

auto load = [](smkflow::ActionContext* context) {
  smkflow::JSON json;
  std::ifstream("save.json") >> json;
  context->board()->Deserialize(json);
};

auto menu = {
    Menu("File",
         {
             MenuEntry("Load", load),
             MenuEntry("Save", save),
         }),
    Menu("Primitive",
         {
             MenuEntry("Cube", CreateCubeNode()),
             MenuEntry("Sphere", CreateNode(model_sphere)),
             MenuEntry("Torus", CreateNode(model_torus)),
             MenuEntry("Capsule", CreateNode(model_capsule)),
         }),
    Menu("Values",
         {
             MenuEntry("Slider", CreateNode(model_slider)),
             MenuEntry("Time", CreateNode(model_time)),
             MenuEntry("Vec3", CreateNode(model_new_vec3)),
         }),
    Menu("Combine",
         {
             MenuEntry("Union", CreateNode(model_union)),
             MenuEntry("Difference", CreateNode(model_difference)),
             MenuEntry("Intersection", CreateNode(model_intersection)),
             MenuEntry("Complement", CreateNode(model_complement)),
             Menu("Smooth",
                  {
                      MenuEntry("Union", CreateNode(model_smooth_union)),
                      MenuEntry("Difference",
                                CreateNode(model_smooth_difference)),
                      MenuEntry("Intersection",
                                CreateNode(model_smooth_intersection)),
                  }),
         }),
    Menu("Transform",
         {
             MenuEntry("Color", CreateNode(model_color)),
             MenuEntry("Repeat", CreateNode(model_repeat)),
             MenuEntry("Scale", CreateNode(model_scale)),
             MenuEntry("Translate", CreateTranslateNode()),
             MenuEntry("Rotate", CreateNode(model_rotate)),
         }),
    Menu("Math",
         {
             Menu("Operations",
                  {
                      MenuEntry("Add", CreateNode(model_math_add)),
                      MenuEntry("Substract", CreateNode(model_math_sub)),
                      MenuEntry("Multiply", CreateNode(model_math_mul)),
                      MenuEntry("Divide", CreateNode(model_math_div)),
                  }),
             Menu("Functions",
                  {
                      MenuEntry("Sin", CreateNode(model_math_sin)),
                      MenuEntry("Cos", CreateNode(model_math_cos)),
                  }),
         }),
    MenuEntry("screen", CreateNode(model_screen)),
};

auto my_board = smkflow::model::Board{
    {
        model_cube,
        model_sphere,
        model_torus,
        model_capsule,
        model_slider,
        model_time,
        model_new_vec3,
        model_union,
        model_difference,
        model_intersection,
        model_complement,
        model_smooth_union,
        model_smooth_difference,
        model_smooth_intersection,
        model_color,
        model_repeat,
        model_scale,
        model_translate,
        model_rotate,
        model_math_add,
        model_math_sub,
        model_math_mul,
        model_math_div,
        model_math_sin,
        model_math_cos,
        model_screen,
    },
    {menu},
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

  int step = 0;

  std::string previous_shader_code;
  window.ExecuteMainLoop([&] {
    g_time = window.time();

    window.shader_program_2d()->Use();
    window.PoolEvents();
    window.Clear(smkflow::color::background);

    board->Step(&window, &window.input());
    board->Draw(&window);

    window.Display();

    Build(board.get(), step);
    step++;
  });
  return EXIT_SUCCESS;
}
