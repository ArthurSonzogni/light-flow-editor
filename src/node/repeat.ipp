// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_repeat = smkflow::model::Node{
    Node::Repeat,
    "Repeat",
    model_type_transformation,
    {
        {"a", type_sdf},
        {"space", type_vec3},
    },
    {
        {"out", type_sdf},
    },
};

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
