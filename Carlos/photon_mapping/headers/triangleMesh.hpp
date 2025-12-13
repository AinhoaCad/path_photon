#ifndef TRIANGLEMESH_HPP
#define TRIANGLEMESH_HPP

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
#include "triangle.hpp"
#include "primitive.hpp"
#include <vector>

using namespace std;

class TriangleMesh: public Primitive {
    vector<Triangle*> triangles;
public:
    TriangleMesh(vector<Triangle*> _triangles,Material _material);
    bool doesIntersect(Ray r, Intersection &intersection) override;
};

#endif