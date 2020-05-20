// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_scale = smkflow::model::Node{
    Node::Scale,
    "scale",
    model_type_transformation,
    {
        {"in", type_sdf},
        {"factor", type_float},
    },
    {
        {"", type_sdf},
    },
};

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
      inner_b, scaled_in, in, out_b, inner_a, out, out_b);
}
