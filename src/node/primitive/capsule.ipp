#include <iostream>
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_capsule = smkflow::model::Node{
    Node::Capsule,
    "Capsule",
    model_type_primitive,
    {
        {"a", type_vec3},
        {"b", type_vec3},
        {"r", type_float},
    },
    {
        {"out", type_sdf},
    },
};

std::string BuildCapsule(smkflow::Node* node,
                         const std::string& in,
                         const std::string out,
                         Context* context) {
  context->RegisterFunction(R"(
    Value Capsule(vec3 a, vec3 b, float radius, vec3 pos) {
      Value value;
      vec3 pa = pos - a;
      vec3 ba = b - a;
      float distance = dot(pa,ba) / dot(ba,ba);
      float distance_clamped = clamp(distance, 0.0, 1.0 );
      value.distance = length( pa - ba * distance_clamped ) - radius;
      value.color = vec3(1.f, 1.f, 1.f);
      return value;
    }
  )");

  std::string compute_a = "";
  std::string var_a = "vec3(2.f, 0.f, 0.f)";
  if (smkflow::Node* node_a = node->InputAt(0)->OppositeNode()) {
    var_a = context->Identifier();
    compute_a = BuildVec3(node_a, var_a, context) + "\n";
  }

  std::string compute_b = "";
  std::string var_b = "vec3(-2.f, 0.f, 0.f)";
  if (smkflow::Node* node_b = node->InputAt(1)->OppositeNode()) {
    var_b = context->Identifier();
    compute_b = BuildVec3(node_b, var_b, context) + "\n";
  }

  std::string compute_r = "";
  std::string var_r = "0.5f";
  if (smkflow::Node* node_r = node->InputAt(2)->OppositeNode()) {
    var_r = context->Identifier();
    compute_r = BuildFloat(node_r, var_r, context) + "\n";
  }

  return fmt::format("{}{}{}  Value {} = Capsule({}, {}, {}, {});", compute_a,
                     compute_b, compute_r, out, var_a, var_b, var_r, in);
}
