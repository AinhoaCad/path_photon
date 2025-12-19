#pragma once
#include "AABB.hpp"
#include "Primitive.hpp"
#include <vector>

class BVHNode {
public:
    AABB box;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    std::vector<Primitive*> primitives;

    BVHNode(std::vector<Primitive*>& objects, int depth = 0);
    bool intersect(Ray& ray, float& t, Primitive*& hit) const;
};
