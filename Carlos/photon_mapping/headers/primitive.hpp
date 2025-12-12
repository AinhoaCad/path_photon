#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP
#include "ray.hpp"
#include "intersection.hpp"
#include "RGB.hpp"
#include "material.hpp"


class Primitive{
    public:
        Material material;

        Primitive(Material _material);
        virtual bool doesIntersect(Ray r, Intersection &intersection) = 0;
        RGB getK_d();
        RGB getK_s();
        RGB getK_t();
        RGB getK_e();
        float getRefractionIndex();
        
};
#endif