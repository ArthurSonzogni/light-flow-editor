// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_sphere = smkflow::model::Node{
    Node::Sphere,
    "Sphere",
    model_type_primitive,
    {},
    {
        {"out", type_sdf},
    },
};

std::string BuildSphere(smkflow::Node*,
                        const std::string& in,
                        const std::string& out,
                        Context* context) {
  context->RegisterFunction(R"(
    Value Sphere(float radius, vec3 pos) {
      Value value;
      value.distance = length(pos) - radius;
      value.color = vec3(1.0, 1.0, 1.0);
      return value;
    }
  )");

  return fmt::format("  Value {} = Sphere(1.f, {});", out, in);
}
