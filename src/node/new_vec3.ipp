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
  float DX = smkflow::Slider(box->ChildAt(0))->GetValue();
  float DY = smkflow::Slider(box->ChildAt(1))->GetValue();
  float DZ = smkflow::Slider(box->ChildAt(2))->GetValue();

  return fmt::format("  vec3 {} = vec3({},{},{});", out, DX, DY, DZ);
}
