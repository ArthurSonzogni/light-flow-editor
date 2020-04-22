auto model_union = smkflow::model::Node{
    Node::Union,
    "Union",
    model_type_fusion,
    {
        {"a", type_sdf},
        {"b", type_sdf},
    },
    {
        {"a+b", type_sdf},
    },
};

std::string BuildUnion(smkflow::Node* node,
                       const std::string& in,
                       const std::string& out,
                       Context* context) {
  context->RegisterFunction(R"(
    Value Union(Value a, Value b) {
      Value value;
      value.distance = min(a.distance, b.distance);
      float lambda = clamp(abs(a.distance) / (abs(a.distance) + abs(b.distance)), 0.f, 1.f);
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
  return fmt::format("{}\n{}\n  Value {} = Union({},{});",  //
                     inner_a, inner_b, out, out_a, out_b);
}
