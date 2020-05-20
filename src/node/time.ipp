// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_time = smkflow::model::Node{
    Node::Time,
    "time",
    model_type_primitive,
    {},
    {
        {"", type_float},
    },
};

std::string BuildTime(smkflow::Node* node,
                      const std::string& out,
                      Context* context) {
  return fmt::format("  float {} = time;", out);
}
