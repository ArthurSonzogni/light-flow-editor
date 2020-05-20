// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_cube = smkflow::model::Node{
    Node::Cube,
    "Cube",
    model_type_primitive,
    {
        {"dim", type_vec3},
    },
    {
        {"", type_sdf},
    },
};

std::string BuildCube(smkflow::Node* node,
                      const std::string& in,
                      const std::string out,
                      Context* context) {
  context->RegisterFunction(R"(
    Value Cube(vec3 size, vec3 pos) {
      vec3 q = abs(pos) - size;
      vec3 clamped = clamp(pos, - size * 0.5, size * 0.5);
      Value value;
      value.distance = length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
      value.color = vec3(1.0, 1.0, 1.0);
      return value;
    }
  )");
  std::string compute_dimensions = "";
  std::string dimension = "vec3(0.8f, 0.8f, 0.8f)";
  if (smkflow::Node* node_dimension = node->InputAt(0)->OppositeNode()) {
    dimension = context->Identifier();
    compute_dimensions = BuildVec3(node_dimension, dimension, context) + "\n";
  }

  return fmt::format("{}  Value {} = Cube({}, {});", compute_dimensions, out,
                     dimension, in);
}

smkflow::Action CreateCubeNode() {
  return [=](smkflow::ActionContext* context) {
    auto* cube = context->board()->Create(model_cube);
    auto* dimension = context->board()->Create(model_new_vec3);
    context->board()->Connect(dimension->OutputAt(0), cube->InputAt(0));

    cube->SetPosition(context->cursor());
    dimension->SetPosition(context->cursor() - glm::vec2(350.f, 0.f));

    auto* box = smkflow::Box(dimension->widget());
    for (int i = 0; i < 3; ++i)
      smkflow::Slider(box->ChildAt(i))->SetValue(0.8f);
  };
}
