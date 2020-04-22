// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_slider = smkflow::model::Node{
    Node::Slider,
    "slider",
    model_type_primitive,
    {},
    {
        {"", type_float},
    },
    smkflow::Slider(-10.f, 10.f, 0.f, "{:.2f}"),
};

std::string BuildSlider(smkflow::Node* node,
                        const std::string& out,
                        Context* context) {
  float value = smkflow::Slider(node->widget())->GetValue();
  return "  float " + out + " = " + std::to_string(value) + ";\n";
}
