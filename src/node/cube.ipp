auto model_cube = smkflow::model::Node{
    Node::Cube,
    "Cube",
    model_type_primitive,
    {
        {"dim", type_vec3},
    },
    {
        {"out", type_sdf},
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
  std::string dimensions = "vec3(0.8f, 0.8f, 0.8f)";
  if (smkflow::Node* node_dimension = node->InputAt(0)->OppositeNode()) {
    dimensions = context->Identifier();
    compute_dimensions = BuildVec3(node_dimension, dimensions, context) + "\n";
  }

  return fmt::format("{}  Value {} = Cube({}, {});", compute_dimensions, out,
                     dimensions, in);
}