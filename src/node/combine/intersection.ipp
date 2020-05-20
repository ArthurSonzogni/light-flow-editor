// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_intersection = smkflow::model::Node{
    Node::Intersection,
    "Intersection",
    model_type_fusion,
    {
        {"", type_sdf},
        {"", type_sdf},
    },
    {
        {"", type_sdf},
    },
};

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
