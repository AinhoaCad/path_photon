// AABB implementation
#include "AABB.hpp"

bool AABB::intersect(const Ray& ray, float tMin, float tMax) const {
    for (int i = 0; i < 3; ++i) {
        float invD = 1.0f / ray.direction_[i];
        float t0 = (min[i] - ray.origin_[i]) * invD;
        float t1 = (max[i] - ray.origin_[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tMin = std::max(t0, tMin);
        tMax = std::min(t1, tMax);
        if (tMax <= tMin) return false;
    }
    return true;
}

AABB AABB::surroundingBox(const AABB& a, const AABB& b) {
    Vector3 small(
        std::min(a.min.x, b.min.x),
        std::min(a.min.y, b.min.y),
        std::min(a.min.z, b.min.z)
    );
    Vector3 big(
        std::max(a.max.x, b.max.x),
        std::max(a.max.y, b.max.y),
        std::max(a.max.z, b.max.z)
    );
    return AABB(small, big);
}
