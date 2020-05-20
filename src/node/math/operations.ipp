// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_math_add = smkflow::model::Node{
    Node::MathAdd,
    "Add",
    model_type_math,
    {
        {"", type_float},
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

auto model_math_sub = smkflow::model::Node{
    Node::MathSub,
    "Substract",
    model_type_math,
    {
        {"", type_float},
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

auto model_math_mul = smkflow::model::Node{
    Node::MathMul,
    "Multiply",
    model_type_math,
    {
        {"", type_float},
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

auto model_math_div = smkflow::model::Node{
    Node::MathDiv,
    "Divide",
    model_type_math,
    {
        {"", type_float},
        {"", type_float},
    },
    {
        {"", type_float},
    },
};

std::string BuildMathAdd(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* node_b = node->InputAt(1)->OppositeNode();
  auto var_b = context->Identifier();
  auto compute_a = BuildFloat(node_a, out, context);
  auto compute_b = BuildFloat(node_b, var_b, context);
  return fmt::format("{}\n{}\n  {} += {};", compute_a, compute_b, out, var_b);
}

std::string BuildMathSub(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* node_b = node->InputAt(1)->OppositeNode();
  auto var_b = context->Identifier();
  auto compute_a = BuildFloat(node_a, out, context);
  auto compute_b = BuildFloat(node_b, var_b, context);
  return fmt::format("{}\n{}\n  {} -= {};", compute_a, compute_b, out, var_b);
}

std::string BuildMathMul(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* node_b = node->InputAt(1)->OppositeNode();
  auto var_b = context->Identifier();
  auto compute_a = BuildFloat(node_a, out, context);
  auto compute_b = BuildFloat(node_b, var_b, context);
  return fmt::format("{}\n{}\n  {} *= {};", compute_a, compute_b, out, var_b);
}

std::string BuildMathDiv(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  smkflow::Node* node_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* node_b = node->InputAt(1)->OppositeNode();
  auto var_b = context->Identifier();
  auto compute_a = BuildFloat(node_a, out, context);
  auto compute_b = BuildFloat(node_b, var_b, context);
  return fmt::format("{}\n{}\n  {} /= {};", compute_a, compute_b, out, var_b);
}
