// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_new_vec3 = smkflow::model::Node{
    Node::NewVec3,
    "vec3",
    model_type_fusion,
    {
        {"x", type_float},
        {"y", type_float},
        {"z", type_float},
    },
    {
        {"out", type_vec3},
    },
    smkflow::VBox({
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
        smkflow::Slider(-5.f, 5.f, 0.f, "{:.2f}"),
    }),
};

std::string BuildNewVec3(smkflow::Node* node,
                         const std::string& out,
                         Context* context) {
  auto box = smkflow::Box(node->widget());

  std::string DX =
      fmt::format("{}", smkflow::Slider(box->ChildAt(0))->GetValue());
  std::string DY =
      fmt::format("{}", smkflow::Slider(box->ChildAt(1))->GetValue());
  std::string DZ =
      fmt::format("{}", smkflow::Slider(box->ChildAt(2))->GetValue());

  std::string compute_dx;
  std::string compute_dy;
  std::string compute_dz;

  if (auto* in = node->InputAt(0)->OppositeNode()) {
    DX = context->Identifier();
    compute_dx = BuildFloat(in, DX, context) + "\n";
  }
  if (auto* in = node->InputAt(1)->OppositeNode()) {
    DY = context->Identifier();
    compute_dy = BuildFloat(in, DY, context) + "\n";
  }
  if (auto* in = node->InputAt(2)->OppositeNode()) {
    DZ = context->Identifier();
    compute_dz = BuildFloat(in, DZ, context) + "\n";
  }

 return fmt::format("{}{}{}  vec3 {} = vec3({},{},{});", compute_dx,
                     compute_dy, compute_dz, out, DX, DY, DZ);
}
