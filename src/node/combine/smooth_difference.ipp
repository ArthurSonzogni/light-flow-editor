// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_smooth_difference = smkflow::model::Node{
    Node::SmoothDifference,
    "Smooth Difference",
    model_type_fusion,
    {
        {"", type_sdf},
        {"", type_sdf},
    },
    {
        {"", type_sdf},
    },
    smkflow::Slider(0.f, 100, 50, "{:.0f}%"),
};

std::string BuildSmoothDifference(smkflow::Node* node,
                               const std::string& in,
                               const std::string& out,
                               Context* context) {
  context->RegisterFunction(R"(
    Value SmoothDifference(float k, Value a, Value b) {
      Value value;
      float lambda = clamp(0.5 - 0.5 * (b.distance + a.distance) / k, 0.0, 1.0);
      value.distance = mix(b.distance, -a.distance, lambda) + k*lambda*(1.0-lambda);
      b.color = b.color * b.color;
      a.color = a.color * a.color;
      value.color = mix(b.color, a.color, lambda);
      value.color = sqrt(value.color);
      return value;
    }
  )");

  float smooth = smkflow::Slider(node->widget())->GetValue() * 0.05;
  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();
  auto out_a = context->Identifier();
  auto out_b = context->Identifier();
  auto inner_a = BuildSDF(input_a, in, out_a, context);
  auto inner_b = BuildSDF(input_b, in, out_b, context);
  return fmt::format(
      "{}\n"
      "{}\n"
      "Value {} = SmoothDifference({}, {},{});",
      inner_a, inner_b, out, smooth, out_a, out_b);
}
