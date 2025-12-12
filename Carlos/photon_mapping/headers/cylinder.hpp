#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <array>
#include "point.hpp"
#include "direction.hpp"
#include "transformations.hpp"
#include "RGB.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "primitive.hpp"

using namespace std;

class Cylinder: public Primitive {
    Direction direction;
    Point center;
    float heigth;
    float radius;

public:
    Cylinder (Direction d, Point c, float h,float r, Material _material);
    bool doesIntersect(Ray r, Intersection &intersection) override;
};

#endif