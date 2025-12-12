#ifndef SPHERE_HPP
#define SPHERE_HPP

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

class Sphere: public Primitive {
    Point center;
    Direction axis;

public:
    Sphere(Point c, Direction a, Material _material);
    Point getCenter();
    Direction getAxis();
    bool doesIntersect(Ray r, Intersection &intersection) override;
    float radius();
};

#endif