// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_number = smkflow::model::Node{
    Node::Number,
    "number",
    model_type_primitive,
    {},
    {
        {"out", type_float},
    },
    smkflow::Slider(-10.f, 10.f, 0.f, "{:.2f}"),
};

std::string BuildNumber(smkflow::Node* node,
                        const std::string& out,
                        Context* context) {
  float value = smkflow::Slider(node->widget())->GetValue();
  return "  float " + out + " = " + std::to_string(value) + ";\n";
}
