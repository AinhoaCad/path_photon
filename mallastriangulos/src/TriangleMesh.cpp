#include "TriangleMesh.hpp"

void TriangleMesh::addTriangle(std::unique_ptr<Triangle> t) {
    triangles.push_back(std::move(t));
}
