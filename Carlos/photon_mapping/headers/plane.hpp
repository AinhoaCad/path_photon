#ifndef PLANE_HPP
#define PLANE_HPP

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
#include "material.hpp"

using namespace std;

class Plane: public Primitive {
    Direction normal;
    float distancetoOrigin;

public:
    Plane(Direction n, float d, Material _material);
    bool doesIntersect(Ray r, Intersection &intersection) override;
};

#endif