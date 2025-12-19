#include "BVHNode.hpp"
#include <algorithm>

BVHNode::BVHNode(std::vector<Primitive*>& objects, int depth) {
    if (objects.size() <= 2) {
        primitives = objects;
        box = objects[0]->boundingBox();
        for (size_t i = 1; i < objects.size(); ++i)
            box = AABB::surroundingBox(box, objects[i]->boundingBox());
        return;
    }

    int axis = depth % 3;
    std::sort(objects.begin(), objects.end(),
        [axis](Primitive* a, Primitive* b) {
            return a->boundingBox().min[axis] < b->boundingBox().min[axis];
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
