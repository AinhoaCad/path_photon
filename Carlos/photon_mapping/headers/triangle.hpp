#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

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

class Triangle: public Primitive {
    Direction normal;
    Point v1,v2,v3;
public:
    Triangle(Point _v1, Point _v2, Point _v3,Material _material);
    bool doesIntersect(Ray r, Intersection &intersection) override;
};

#endif