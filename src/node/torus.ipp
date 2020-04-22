auto model_torus = smkflow::model::Node{
    Node::Torus,
    "Torus",
    model_type_primitive,
    {
        {"r1", type_float},
        {"r2", type_float},
    },
    {
        {"out", type_sdf},
    },
};

std::string BuildTorus(smkflow::Node* node,
                      const std::string& in,
                      const std::string out,
                      Context* context) {
  context->RegisterFunction(R"(
    Value Torus(float radius_1, float radius_2, vec3 pos) {
      Value value;
      vec2 q = vec2(length(pos.xz)-radius_1,pos.y);
      value.distance = length(q)-radius_2;
      value.color = vec3(1.0, 1.0, 1.0);
      return value;
    }
  )");

  std::string compute_radius_1 = "";
  std::string compute_radius_2 = "";
  std::string radius_1_var = "2.f";
  std::string radius_2_var = "0.2f";

  if (smkflow::Node* node_radius_1 = node->InputAt(0)->OppositeNode()) {
    radius_1_var = context->Identifier();
    compute_radius_1 = BuildFloat(node_radius_1, radius_1_var, context) + "\n";
  }

  if (smkflow::Node* node_radius_2 = node->InputAt(1)->OppositeNode()) {
    radius_2_var = context->Identifier();
    compute_radius_2 = BuildFloat(node_radius_2, radius_2_var, context) + "\n";
  }

  return fmt::format("{}{}  Value {} = Torus({}, {}, {});",  //
                     compute_radius_1, compute_radius_2,      //
                     out, radius_1_var, radius_2_var, in);
}
