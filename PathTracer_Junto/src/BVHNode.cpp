#include "BVHNode.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr size_t kLeafSize = 4; // pequeño para meshes, reduce recursión

float centroidAxis(const Primitive* p, int axis) {
    const AABB bb = p->boundingBox();
    const float c = (bb.min[axis] + bb.max[axis]) * 0.5f;
    return c;
}
}

BVHNode::BVHNode(std::vector<Primitive*>& objects, int depth) {
    if (objects.empty()) {
        return;
    }

    if (objects.size() <= kLeafSize) {
        primitives = objects;
        box = objects[0]->boundingBox();
        for (size_t i = 1; i < objects.size(); ++i) {
            box = AABB::surroundingBox(box, objects[i]->boundingBox());
        }
        return;
    }

    // Elegir eje por mayor varianza de centroides
    std::array<float, 3> mean{0.f, 0.f, 0.f};
    for (auto* p : objects) {
        mean[0] += centroidAxis(p, 0);
        mean[1] += centroidAxis(p, 1);
        mean[2] += centroidAxis(p, 2);
    }
    const float invN = 1.0f / static_cast<float>(objects.size());
    mean[0] *= invN; mean[1] *= invN; mean[2] *= invN;

    std::array<float, 3> var{0.f, 0.f, 0.f};
    for (auto* p : objects) {
        const float cx = centroidAxis(p, 0);
        const float cy = centroidAxis(p, 1);
        const float cz = centroidAxis(p, 2);
        var[0] += (cx - mean[0]) * (cx - mean[0]);
        var[1] += (cy - mean[1]) * (cy - mean[1]);
        var[2] += (cz - mean[2]) * (cz - mean[2]);
    }

    int axis = 0;
    if (var[1] > var[axis]) axis = 1;
    if (var[2] > var[axis]) axis = 2;

    std::sort(objects.begin(), objects.end(),
        [axis](Primitive* a, Primitive* b) {
            return centroidAxis(a, axis) < centroidAxis(b, axis);
        });

    size_t mid = objects.size() / 2;
    std::vector<Primitive*> leftObjs(objects.begin(), objects.begin() + mid);
    std::vector<Primitive*> rightObjs(objects.begin() + mid, objects.end());

    left = new BVHNode(leftObjs, depth + 1);
    right = new BVHNode(rightObjs, depth + 1);

    box = AABB::surroundingBox(left->box, right->box);
}

bool BVHNode::intersect(Ray& ray, float& t, Primitive*& hit) const {
    if (!box.intersect(ray, 0.001f, t)) return false;

    bool hitSomething = false;

    if (!primitives.empty()) {
        for (auto* p : primitives) {
            float tempT;
            if (p->intersect(ray, tempT) && tempT < t) {
                t = tempT;
                hit = p;
                hitSomething = true;
            }
        }
        return hitSomething;
    }

    bool hitLeft = left && left->intersect(ray, t, hit);
    bool hitRight = right && right->intersect(ray, t, hit);

    return hitLeft || hitRight;
}
