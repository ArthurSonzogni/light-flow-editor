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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file./
std::string BuildScreen(smkflow::Node* node, Context* context) {
  smkflow::Node* input_node = node->InputAt(0)->OppositeNode();
  std::string in = context->Identifier();
  std::string out = context->Identifier();
  std::string inner = BuildSDF(input_node, in, out, context);
  return fmt::format(
      "{}\n"
      "Value sdf(vec3 {}) {{\n"
      "{}\n"
      "  return {};\n"
      "}}",
      context->RegisteredFunction(), in, inner, out);
}
