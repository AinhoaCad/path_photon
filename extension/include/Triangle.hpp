#pragma once
#include "Primitive.hpp"

class Triangle : public Primitive {
public:
    Point v0, v1, v2;
    Direction n0, n1, n2;
    Material material;

    Triangle(const Point& a, const Point& b, const Point& c,
             const Direction& na, const Direction& nb, const Direction& nc,
             const Material& m)
        : v0(a), v1(b), v2(c), n0(na), n1(nb), n2(nc), material(m) {}

    bool intersect(Ray& ray, float& t) const override;
    Material getMaterial() const override { return material; }
    Direction getPointNormal(Point p) const override;
    bool emitsLight() const override { return false; }

    AABB boundingBox() const override;
};
