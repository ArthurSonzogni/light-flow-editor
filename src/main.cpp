#include <smk/Color.hpp>
#include <smk/Window.hpp>
#include <smkflow/Elements.hpp>
#include <smkflow/Model.hpp>
#include <smkflow/Widget.hpp>

enum Node {
  Screen,

  Number,
  Time,

  Vec3,

  Union,
  Intersection,
  Complement,
  Difference,

  Sphere,
  Torus,
  Cube,

  Translate,
  Scale,
};

auto type_float = glm::vec4{0.7f, 0.7f, 1.f, 1.f};
auto type_vec3 = glm::vec4{0.7f, 1.f, 1.f, 1.f};
auto type_sdf = glm::vec4{1.0f, 0.7f, 0.7f, 1.f};

auto node_type_fusion = glm::vec4{1.0, 0.8, 0.8, 1.0f};
auto node_type_screen = glm::vec4{1.0, 1.0, 1.0, 1.0f};
auto node_type_primitive = glm::vec4{0.8, 1.0, 0.8, 1.0f};
auto node_type_transformation = glm::vec4{0.8, 0.8, 1.0, 1.0f};

auto node_screen = smkflow::model::Node{
    Node::Screen,
    "Screen",
    node_type_screen,
    {
        {"in", type_sdf},
    },
    {},
    {},
};

auto node_number = smkflow::model::Node{
    Node::Number,
    "number",
    node_type_primitive,
    {},
    {},
    {
        {"out", type_float},
    },
};

auto node_time = smkflow::model::Node{
    Node::Time,
    "time",
    node_type_primitive,
    {},
    {},
    {
        {"out", type_float},
    },
};

auto node_cube = smkflow::model::Node{
    Node::Cube,
    "Cube",
    node_type_primitive,
    {},
    {},
    {
        {"out", type_sdf},
    },
};

auto node_sphere = smkflow::model::Node{
    Node::Sphere,
    "Sphere",
    node_type_primitive,
    {},
    {},
    {
        {"out", type_sdf},
    },
};

auto node_torus = smkflow::model::Node{
    Node::Torus,
    "Torus",
    node_type_primitive,
    {},
    {},
    {
        {"out", type_sdf},
    },
};

auto node_vec3 = smkflow::model::Node{
    Node::Vec3,
    "vec3",
    node_type_fusion,
    {
      {"x", type_float},
      {"y", type_float},
      {"z", type_float},
    },
    {
        smkflow::InputBox::Create(0.f),
        smkflow::InputBox::Create(0.f),
        smkflow::InputBox::Create(0.f),
    },
    {
        {"out", type_vec3},
    },
};

auto node_translate = smkflow::model::Node{
    Node::Translate,
    "translate",
    node_type_transformation,
    {
        {"in", type_sdf},
        {"translate", type_vec3},
    },
    {},
    {
        {"out", type_sdf},
    },
};

auto node_scale = smkflow::model::Node{
    Node::Scale,
    "scale",
    node_type_transformation,
    {
        {"in", type_sdf},
        {"factor", type_float},
    },
    {},
    {
        {"out", type_sdf},
    },
};

auto node_union = smkflow::model::Node{
    Node::Union,
    "Union",
    node_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
    {},
    {
        {"a+b", type_sdf},
    },
};

auto node_intersection = smkflow::model::Node{
    Node::Intersection,
    "Intersection",
    node_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
    {},
    {
        {"a.b", type_sdf},
    },
};

auto node_difference = smkflow::model::Node{
    Node::Difference,
    "Difference",
    node_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
    {},
    {
        {"a-b", type_sdf},
    },
};

auto node_complement = smkflow::model::Node{
    Node::Complement,
    "Complement",
    node_type_fusion,
    {
        {"a", type_sdf},
    },
    {},
    {
        {"-a", type_sdf},
    },
};

auto my_board = smkflow::model::Board{
    {
        node_screen,
        node_number,
        node_time,
        node_vec3,
        node_union,
        node_intersection,
        node_complement,
        node_difference,
        node_sphere,
        node_torus,
        node_cube,
        node_translate,
        node_scale,
    },
    "../resources/arial.ttf",
};

int main() {
  // Open a new window.
  auto window = smk::Window(512, 512, "test");
  auto board = smkflow::Board::Create(my_board);

  // Instanciate some Node based on the model.
  int x = -my_board.nodes.size() / 2;
  for (const auto& node_model : my_board.nodes) {
    for (int y = -4; y < 4; ++y) {
      smkflow::Node* node = board->Create(node_model);
      node->SetPosition({200 * x, 200 * y});
    }
    ++x;
  }

  window.ExecuteMainLoop([&] {
    window.PoolEvents();
    window.Clear({0.2, 0.2, 0.2, 1.0});
    board->Step(&window, &window.input());
    board->Draw(&window);
    window.Display();
  });
  return EXIT_SUCCESS;
}
