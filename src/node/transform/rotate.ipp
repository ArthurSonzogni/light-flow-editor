// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_rotate = smkflow::model::Node{
    Node::Rotate,
    "rotate",
    model_type_transformation,
    {
        {"n", type_sdf},
        {"direction", type_vec3},
        {"angle", type_float},
    },
    {
        {"", type_sdf},
    },
};

std::string BuildRotate(smkflow::Node* node,
                           const std::string& in,
                           const std::string& out,
                           Context* context) {
  context->RegisterFunction(R"(
    vec3 Rotate(vec3 pos, vec3 direction, float angle) {
      direction = normalize(direction);
      vec3 c  = dot(pos, direction) * direction;
      vec3 a = pos - c;
      vec3 b = cross(a, direction);
      return cos(angle) * a + sin(angle) * b + c;
    }
  )");

  smkflow::Node* node_in = node->InputAt(0)->OppositeNode();
  smkflow::Node* node_direction = node->InputAt(1)->OppositeNode();
  smkflow::Node* node_angle = node->InputAt(2)->OppositeNode();

  auto var_rotated = context->Identifier();
  auto var_direction = context->Identifier();
  auto var_angle = context->Identifier();

  auto compute_in = BuildSDF(node_in, var_rotated, out, context);
  auto compute_direction = BuildVec3(node_direction, var_direction, context);
  auto compute_angle = BuildFloat(node_angle, var_angle, context);

  return fmt::format(                             //
      "{}\n"                                      //
      "{}\n"                                      //
      "  vec3 {} = Rotate({},{},{});\n"           //
      "{}\n",                                     //
      compute_direction,                          //
      compute_angle,                              //
      var_rotated, in, var_direction, var_angle,  //
      compute_in);                                //
}
