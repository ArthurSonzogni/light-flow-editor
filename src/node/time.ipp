auto model_time = smkflow::model::Node{
    Node::Time,
    "time",
    model_type_primitive,
    {},
    {
        {"out", type_float},
    },
};
