#include "triangleMesh.hpp"


TriangleMesh::TriangleMesh(vector<Triangle *> _triangles, Material _material): Primitive(_material), triangles(_triangles)
{
}

bool TriangleMesh::doesIntersect(Ray r, Intersection &intersection)
{
    for (Triangle* triangle: triangles){
        if (triangle->doesIntersect(r,intersection)){
            return true;
        }
    }
    return false;
}
