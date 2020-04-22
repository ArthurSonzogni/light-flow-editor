auto model_complement = smkflow::model::Node{
    Node::Complement,
    "Complement",
    model_type_fusion,
    {
        {"a", type_sdf},
    },
    {
        {"-a", type_sdf},
    },
};

