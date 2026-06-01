

template <typename Effect>
struct AudioParameter{
    std::string name;
    std::string id;
    float min;
    float max;
    float default_value;
    void (Effect::*setParameter)(float); 
};

/*
    void (effect::*setParameter)(float) : pointer to a void function called setParameter which is 
    a member function of effect class, and takes in a float value which sets the parameter's value.
*/