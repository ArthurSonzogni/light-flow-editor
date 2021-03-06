// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_translate = smkflow::model::Node{
    Node::Translate,
    "translate",
    model_type_transformation,
    {
        {"in", type_sdf},
        {"translate", type_vec3},
    },
    {
        {"", type_sdf},
    },
};

std::string BuildTranslate(smkflow::Node* node,
                           const std::string& in,
                           const std::string& out,
                           Context* context) {
  smkflow::Node* input_a = node->InputAt(0)->OppositeNode();
  smkflow::Node* input_b = node->InputAt(1)->OppositeNode();

  auto in_translated = context->Identifier();
  auto translation = context->Identifier();

  auto inner_a = BuildSDF(input_a, in_translated, out, context);
  auto inner_b = BuildVec3(input_b, translation, context);
  return fmt::format(
      "{}\n"
      "  vec3 {} = {} - {};\n"
      "{}\n",  //
      inner_b, in_translated, in, translation, inner_a);
}

smkflow::Action CreateTranslateNode() {
  return [=](smkflow::ActionContext* context) {
    auto* translate = context->board()->Create(model_translate);
    auto* delta = context->board()->Create(model_new_vec3);
    context->board()->Connect(delta->OutputAt(0), translate->InputAt(1));

    translate->SetPosition(context->cursor());
    delta->SetPosition(context->cursor() + glm::vec2(-300.f, +50.f));
  };
}
