#include <fmt/core.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <smk/Color.hpp>
#include <smk/Framebuffer.hpp>
#include <smk/Shader.hpp>
#include <smk/Shape.hpp>
#include <smk/Sprite.hpp>
#include <smk/Window.hpp>
#include <smkflow/Elements.hpp>
#include <smkflow/Model.hpp>
#include <smkflow/widget/Widget.hpp>
#include <smkflow/widget/Slider.hpp>
#include <smkflow/widget/Label.hpp>
#include <smkflow/widget/Input.hpp>
#include <smkflow/widget/Box.hpp>
#include <smkflow/widget/Menu.hpp>
#include <smkflow/Constants.hpp>
#include <sstream>

float g_time;

class RenderWidget : public smkflow::Widget {
 public:
  static smkflow::WidgetFactory Create() {
    return [](smkflow::Widget::Delegate* delegate) {
      return std::make_unique<RenderWidget>(delegate);
    };
  }

  static RenderWidget* From(smkflow::Widget* node) {
    return dynamic_cast<RenderWidget*>(node);
  }

  RenderWidget(smkflow::Widget::Delegate* delegate)
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

  glm::vec2 ComputeDimensions() override { return {256.f, 256.f}; }

  void Draw(smk::RenderTarget* target) override {
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
    sphere_rotation = glm::rotate(sphere_rotation, -g_time * 0.4f, glm::vec3(0.f, 1.f, 0.f));
    sphere_rotation = glm::translate(sphere_rotation, glm::vec3(0.f, +3.f, -3.f));
    sphere_rotation =
        glm::rotate(sphere_rotation, float(M_PI / 4.f), glm::vec3(1.f, 0.f, 0.f));
    shader_program_.SetUniform("sphere_rotation", sphere_rotation);
    framebuffer_.Draw(square_);

    sprite_.SetPosition(Position());
    target->Draw(sprite_);
  }

  void Build(std::string new_code) {
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

  private:
    float size_ = 256;
    std::string code_;

    smk::Framebuffer framebuffer_;

    smk::ShaderProgram shader_program_;
    smk::Shader vertex_shader_;
    smk::Shader fragment_shader_;

    smk::Sprite sprite_;

    smk::Transformable square_;
};

enum Node {
  Screen,

  Number,
  Time,

  NewVec3,

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

auto node_type_fusion = glm::vec4{0.5, 0.4, 0.4, 1.0f};
auto node_type_screen = glm::vec4{0.5, 0.5, 0.5, 1.0f};
auto node_type_primitive = glm::vec4{0.4, 0.5, 0.4, 1.0f};
auto node_type_transformation = glm::vec4{0.4, 0.4, 0.5, 1.0f};

auto node_screen = smkflow::model::Node{
    Node::Screen,
    "Screen",
    node_type_screen,
    {
        {"in", type_sdf},
    },
    {},
    RenderWidget::Create(),
};

auto node_number = smkflow::model::Node{
    Node::Number,
    "number",
    node_type_primitive,
    {},
    {
        {"out", type_float},
    },
    smkflow::Slider(-10.f, 10.f, 0.f, "{:.2f}"),
};

auto node_time = smkflow::model::Node{
    Node::Time,
    "time",
    node_type_primitive,
    {},
    {
        {"out", type_float},
    },
};

auto node_cube = smkflow::model::Node{
    Node::Cube,
    "Cube",
    node_type_primitive,
    {
        {"dim", type_vec3},
    },
    {
        {"out", type_sdf},
    },
};

auto node_sphere = smkflow::model::Node{
    Node::Sphere,
    "Sphere",
    node_type_primitive,
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
    {
        {"out", type_sdf},
    },
};

auto node_new_vec3 = smkflow::model::Node{
    Node::NewVec3,
    "vec3",
    node_type_fusion,
    {
        {"x", type_float},
        {"y", type_float},
        {"z", type_float},
    },
    {
        {"out", type_vec3},
    },
    smkflow::VBox({
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
    }),
};

auto node_translate = smkflow::model::Node{
    Node::Translate,
    "translate",
    node_type_transformation,
    {
        {"in", type_sdf},
        {"translate", type_vec3},
    },
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
    {
        {"a+b", type_sdf},
    },
};

auto node_smoothed_union = smkflow::model::Node{
    Node::SmoothedUnion,
    "Smooth Union",
    node_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
    {
        {"a+b", type_sdf},
    },
    smkflow::Slider(0.f, 1.f, 0.5f, "Smooth = {:.2f}"),
};

auto node_intersection = smkflow::model::Node{
    Node::Intersection,
    "Intersection",
    node_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
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
    {
        {"-a", type_sdf},
    },
};

auto node_repeat= smkflow::model::Node{
    Node::Repeat,
    "Repeat",
    node_type_transformation,
    {
        {"a", type_sdf},
        {"space", type_vec3},
    },
    {
        {"out", type_sdf},
    },
};

auto my_board = smkflow::model::Board{
    {
        smkflow::MenuEntry("screen", smkflow::CreateNode(node_screen)),
        smkflow::MenuEntry("number", smkflow::CreateNode(node_number)),
        smkflow::MenuEntry("time", smkflow::CreateNode(node_time)),
        smkflow::MenuEntry("new_vec3", smkflow::CreateNode(node_new_vec3)),
        smkflow::MenuEntry("union", smkflow::CreateNode(node_union)),
        smkflow::MenuEntry("smoothed_union",
                           smkflow::CreateNode(node_smoothed_union)),
        smkflow::MenuEntry("intersection",
                           smkflow::CreateNode(node_intersection)),
        smkflow::MenuEntry("complement", smkflow::CreateNode(node_complement)),
        smkflow::MenuEntry("difference", smkflow::CreateNode(node_difference)),
        smkflow::MenuEntry("sphere", smkflow::CreateNode(node_sphere)),
        smkflow::MenuEntry("repeat", smkflow::CreateNode(node_repeat)),
        smkflow::MenuEntry("torus", smkflow::CreateNode(node_torus)),
        smkflow::MenuEntry("cube", smkflow::CreateNode(node_cube)),
        smkflow::MenuEntry("translate", smkflow::CreateNode(node_translate)),
        smkflow::MenuEntry("scale", smkflow::CreateNode(node_scale)),
    },
    "../resources/arial.ttf",
};

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

    void RegisterFunction(const char* f) {
      function_set_.insert(f);
    }

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


std::string BuildCube(smkflow::Node* node, const std::string& in, const std::string out, Context* context) {
  context->RegisterFunction(R"(
    Value Cube(vec3 size, vec3 pos) {
      vec3 q = abs(pos) - size;
      vec3 clamped = clamp(pos, - size * 0.5, size * 0.5);
      Value value;
      value.distance = length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
      value.color = vec3(0.0, 1.0, 0.0);
      return value;
    }
  )");

  std::string compute_dimensions = "";
  std::string dimensions = "vec3(0.8f, 0.8f, 0.8f)";
  if (smkflow::Node* node_dimension = node->InputAt(0)->OppositeNode()) {
    dimensions = context->Identifier();
    compute_dimensions = BuildVec3(node_dimension, dimensions, context) + "\n";
  }

  return fmt::format("{}  Value {} = Cube({}, {});",  //
                     compute_dimensions, out, dimensions, in);
}

std::string BuildSphere(smkflow::Node*, const std::string& in, const std::string& out, Context* context) {
  context->RegisterFunction(R"(
    Value Sphere(float radius, vec3 pos) {
      Value value;
      value.distance = length(pos) - radius;
      value.color = vec3(1.0, 0.0, 0.0);
      return value;
    }
  )");

  return fmt::format("  Value {} = Sphere(1.f, {});", out, in);
}

std::string BuildUnion(smkflow::Node* node,
                       const std::string& in,
                       const std::string& out,
                       Context* context) {
  context->RegisterFunction(R"(
    Value Union(Value a, Value b) {
      Value value;
      value.distance = min(a.distance, b.distance);
      float lambda = clamp(abs(a.distance) / (abs(a.distance) + abs(b.distance)), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }
  )");

  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_a = context->Identifier();
  auto out_b = context->Identifier();
  auto inner_a = BuildSDF(input_a, in, out_a, context);
  auto inner_b = BuildSDF(input_b, in, out_b, context);
  return fmt::format("{}\n{}\n  Value {} = Union({},{});",  //
                     inner_a, inner_b, out, out_a, out_b);
}

std::string BuildSmoothedUnion(smkflow::Node* node,
                               const std::string& in,
                               const std::string& out,
                               Context* context) {
  context->RegisterFunction(R"(
    Value SmoothedUnion(float k, Value a, Value b) {
      Value value;
      float lambda = clamp(0.5 + 0.5 * (b.distance - a.distance) / k, 0.0, 1.0);
      value.distance = mix(b.distance, a.distance, lambda) - k*lambda*(1.0-lambda);
      b.color = b.color * b.color;
      a.color = a.color * a.color;
      value.color = mix(b.color, a.color, lambda);
      value.color = sqrt(value.color);
      return value;
    }
  )");

  float smooth = smkflow::Slider(node->widget())->GetValue();
  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_a = context->Identifier();
  auto out_b = context->Identifier();
  auto inner_a = BuildSDF(input_a, in, out_a, context);
  auto inner_b = BuildSDF(input_b, in, out_b, context);
  return fmt::format(
      "{}\n"
      "{}\n"
      "Value {} = SmoothedUnion({}, {},{});",
      inner_a, inner_b, out, smooth, out_a, out_b);
}

std::string BuildIntersection(smkflow::Node* node,
                       const std::string& in,
                       const std::string& out,
                       Context* context) {
  context->RegisterFunction(R"(
    Value Intersection(Value a, Value b) {
      Value value;
      value.distance = max(a.distance, b.distance);
      float lambda = clamp(a.distance / (a.distance + b.distance), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }
  )");

  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_a = context->Identifier();
  auto out_b = context->Identifier();
  auto inner_a = BuildSDF(input_a, in, out_a, context);
  auto inner_b = BuildSDF(input_b, in, out_b, context);
  return fmt::format("{}\n{}\n  Value {} = Intersection({}, {});",  //
                     inner_a, inner_b, out, out_a, out_b);
}

std::string BuildDifference(smkflow::Node* node,
                       const std::string& in,
                       const std::string& out,
                       Context* context) {
  context->RegisterFunction(R"(
    Value Difference(Value a, Value b) {
      Value value;
      value.distance = max(a.distance, -b.distance);
      float lambda = clamp(a.distance / (a.distance + b.distance), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }
  )");

  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_a = context->Identifier();
  auto out_b = context->Identifier();
  auto inner_a = BuildSDF(input_a, in, out_a, context);
  auto inner_b = BuildSDF(input_b, in, out_b, context);

  return fmt::format("{}\n{}\n  Value {} = Difference({}, {});",  //
                     inner_a, inner_b, out, out_a, out_b);
}

std::string BuildScale(smkflow::Node* node,
                       const std::string& in,
                       const std::string& out,
                       Context* context) {
  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_b = context->Identifier();
  auto scaled_in = context->Identifier();
  auto inner_a = BuildSDF(input_a, scaled_in, out, context);
  auto inner_b = BuildFloat(input_b, out_b, context);
  return fmt::format(
      "{}\n"
      "  vec3 {} = {} / {};\n"
      "{}\n"
      "  {}.distance *= {};",
      inner_b,
      scaled_in, in, out_b,
      inner_a,
      out, out_b);
}

std::string BuildTranslate(smkflow::Node* node,
                           const std::string& in,
                           const std::string& out,
                           Context* context) {
  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();

  auto in_translated = context->Identifier();
  auto translation = context->Identifier();

  auto inner_a = BuildSDF(input_a, in_translated, out, context);
  auto inner_b = BuildVec3(input_b, translation, context);
  return fmt::format(
      "{}\n"
      "  vec3 {} = {} - {};\n"
      "{}\n",  //
      inner_b, in_translated, in, translation, inner_a);
}

std::string BuildRepeat(smkflow::Node* node,
                        const std::string& in,
                        const std::string& out,
                        Context* context) {
  context->RegisterFunction(R"(
    vec3 Repeat(vec3 pos, vec3 size) {
      return mod(pos+0.5* size, size)-0.5* size;
    }
  )");

  smkflow::Node* input_pos = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_dim = node->InputAt(1)->OppositeNode();

  auto out_dimension = context->Identifier();
  auto repeated_in = context->Identifier();

  auto inner_dimension = BuildVec3(input_dim, out_dimension, context);
  auto inner_sdf = BuildSDF(input_pos, repeated_in, out, context);
  return fmt::format(
      "{}\n"
      "  vec3 {} = Repeat({}, {});\n"
      "{}\n",
      inner_dimension, repeated_in, in, out_dimension, inner_sdf);
}

std::string BuildNewVec3(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  auto box = smkflow::Box(node->widget());
  float DX = smkflow::Slider(box->ChildAt(0))->GetValue();
  float DY = smkflow::Slider(box->ChildAt(1))->GetValue();
  float DZ = smkflow::Slider(box->ChildAt(2))->GetValue();

  return fmt::format("  vec3 {} = vec3({},{},{});", out, DX, DY, DZ);
}

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

const char* footer =  R"(

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
    out_color = vec4(1.f, 0.f, 0.f, 1.f);

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

std::string BuildScreen(smkflow::Node* node, Context* context) {
  smkflow::Node* input_node = node->InputAt(0)->OppositeNode();
  std::string in = context->Identifier();
  std::string out = context->Identifier();
  std::string transformed_in = context->Identifier();
  std::string inner = BuildSDF(input_node, transformed_in, out, context);
  return fmt::format(
      "{0}\n"
      "Value sdf(vec3 {1}) {{\n"
      "  vec3 {3} = (sphere_rotation * vec4({2}, 1.0)).xyz;\n"
      "{4}\n"
      "  return {5};\n"
      "}}"
      ,context->RegisteredFunction(), in, in, transformed_in, inner, out);
}

std::string BuildNumber(smkflow::Node* node,
                        const std::string& out,
                        Context* context) {
  float value = smkflow::Slider(node->widget())->GetValue();
  return "  float " + out + " = " + std::to_string(value) + ";\n";
}

std::string BuildSDF(smkflow::Node* node,
                     const std::string& in,
                     const std::string& out,
                     Context* context) {
  if (!node)
    return BuildUnimplemented(context);
  switch (node->Identifier()) {
    case Node::Cube:
      return BuildCube(node, in, out, context);
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

int main() {
  // Open a new window.
  auto window = smk::Window(512, 512, "test");
  auto board = smkflow::Board::Create(my_board);

  // Instanciate some Node based on the model.
  //int x = -my_board.nodes.size() / 2;
  //for (const auto& node_model : my_board.nodes) {
    //for (int y = -2; y <= 1; ++y) {
      //smkflow::Node* node = board->Create(node_model);
      //node->SetPosition({200 * x, 200 * y});
    //}
    //++x;
  //}

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
