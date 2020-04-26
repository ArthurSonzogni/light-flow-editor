// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_math_cos = smkflow::model::Node{
    Node::MathCos,
    "cos",
    model_type_math,
    {
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

auto model_math_sin = smkflow::model::Node{
    Node::MathSin,
    "sin",
    model_type_math,
    {
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

std::string BuildMathCos(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_in = node->InputAt(0)->OppositeNode();
  auto compute_in = BuildFloat(node_in, out, context);
  return fmt::format("{0}\n  {1} = cos({1});", compute_in, out);
}

std::string BuildMathSin(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_in = node->InputAt(0)->OppositeNode();
  auto compute_in = BuildFloat(node_in, out, context);
  return fmt::format("{0}\n  {1} = sin({1});", compute_in, out);
}
