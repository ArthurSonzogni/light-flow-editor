// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

auto model_complement = smkflow::model::Node{
    Node::Complement,
    "Complement",
    model_type_fusion,
    {
        {"", type_sdf},
    },
    {
        {"", type_sdf},
    },
};
