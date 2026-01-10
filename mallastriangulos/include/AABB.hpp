#pragma once
#include "Vector3.hpp"
#include "Ray.hpp"
#include <algorithm>

struct AABB {
    Vector3 min;
    Vector3 max;

    AABB() {}
    AABB(const Vector3& a, const Vector3& b) : min(a), max(b) {}

    bool intersect(const Ray& ray, float tMin, float tMax) const;

    static AABB surroundingBox(const AABB& a, const AABB& b);
};
