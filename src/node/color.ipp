// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_color = smkflow::model::Node{
    Node::Color,
    "Color",
    model_type_primitive,
    {
        {"in", type_sdf},
        {"color", type_vec3},
    },
    {
        {"out", type_sdf},
    },
};

std::string BuildColor(smkflow::Node* node,
                       const std::string& in,
                       const std::string out,
                       Context* context) {
  // Input
  smkflow::Node* input_node = node->InputAt(0)->OppositeNode();
  std::string compute_input = BuildSDF(input_node, in, out, context);

  // Color
  std::string compute_color = "";
  std::string color_var = "vec3(0.2f, 1.0f, 0.8f)";
  if (smkflow::Node* node_color = node->InputAt(1)->OppositeNode()) {
    color_var = context->Identifier();
    compute_color = BuildVec3(node_color, color_var, context) + "\n";
  }

  return fmt::format("{}\n{}  {}.color = {};",  //
                     compute_input, compute_color, out, color_var);
}
