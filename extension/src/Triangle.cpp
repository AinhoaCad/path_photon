#include "Triangle.hpp"
#include <algorithm>
#include <cmath>

bool Triangle::intersect(Ray& ray, float& t) const {
    // Möller–Trumbore
    Vector3 e1 = v1 - v0;
    Vector3 e2 = v2 - v0;
    Vector3 h = ray.direction_.cross(e2);
    float a = e1.dotProduct(h);

    if (fabs(a) < 1e-6f) return false;

    float f = 1.0f / a;
    Vector3 s = ray.origin_ - v0.toVector();
    float u = f * s.dotProduct(h);
    if (u < 0.0f || u > 1.0f) return false;

    Vector3 q = s.cross(e1);
    float v = f * ray.direction_.dotProduct(q);
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * e2.dotProduct(q);
    return t > 0.001f;
}

Direction Triangle::getPointNormal(Point p) const {
    // Si no queremos suavizado, devolver la normal geométrica del triángulo
    if (!smoothNormals) {
        return faceNormal;
    }
    
    // Suavizado: interpolar usando coordenadas baricéntricas
    Vector3 v0v1 = v1 - v0;
    Vector3 v0v2 = v2 - v0;
    Vector3 v0p  = p - v0;

    float d00 = v0v1.dotProduct(v0v1);
    float d01 = v0v1.dotProduct(v0v2);
    float d11 = v0v2.dotProduct(v0v2);
    float d20 = v0p.dotProduct(v0v1);
    float d21 = v0p.dotProduct(v0v2);

    float denom = d00 * d11 - d01 * d01;
    if (std::fabs(denom) < 1e-8f) {
        return faceNormal;
    }
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    Vector3 n = n0.toVector() * u + n1.toVector() * v + n2.toVector() * w;
    float len = n.length();
    if (len < 1e-8f) {
        return faceNormal;
    }
    return n.normalized().toDirection();
}

AABB Triangle::boundingBox() const {
    Vector3 minV(
        std::min({v0.x, v1.x, v2.x}),
        std::min({v0.y, v1.y, v2.y}),
        std::min({v0.z, v1.z, v2.z})
    );
    Vector3 maxV(
        std::max({v0.x, v1.x, v2.x}),
        std::max({v0.y, v1.y, v2.y}),
        std::max({v0.z, v1.z, v2.z})
    );
    // Add epsilon to avoid zero-width boxes that can break traversal.
    const float eps = 1e-4f;
    minV = minV - Vector3(eps, eps, eps);
    maxV = maxV + Vector3(eps, eps, eps);
    return AABB(minV, maxV);
}


