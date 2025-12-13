#include "primitive.hpp"



Primitive::Primitive(Material _material):material(_material)
{
}

RGB Primitive::getK_d()
{
    return material.getK_d();
}
RGB Primitive::getK_s()
{
    return material.getK_s();
}
RGB Primitive::getK_t()
{
    return material.getK_t();
}
RGB Primitive::getK_e()
{
    return material.getK_e();
}

float Primitive::getRefractionIndex()
{
    return material.getRefractionIndex();
}
