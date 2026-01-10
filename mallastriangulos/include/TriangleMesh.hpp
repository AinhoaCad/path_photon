#pragma once
#include <vector>
#include <memory>
#include "Triangle.hpp"

class TriangleMesh {
public:
    std::vector<std::unique_ptr<Triangle>> triangles;

    void addTriangle(std::unique_ptr<Triangle> t);
};
