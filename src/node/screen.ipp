auto model_screen = smkflow::model::Node{
    Node::Screen,
    "Screen",
    model_type_screen,
    {
        {"in", type_sdf},
    },
    {},
    RenderWidget::Create(),
};

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
      "}}",
      context->RegisteredFunction(), in, in, transformed_in, inner, out);
}
