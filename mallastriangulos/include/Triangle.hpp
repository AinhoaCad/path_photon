#pragma once
#include "Primitive.hpp"

class Triangle : public Primitive {
public:
    Point v0, v1, v2;
    Direction n0, n1, n2;
    Material material;
    bool smoothNormals;      // flag para controlar suavizado
    Direction faceNormal;    // normal geométrica del triángulo

    Triangle(const Point& a, const Point& b, const Point& c,
             const Direction& na, const Direction& nb, const Direction& nc,
             const Material& m, bool smooth = true)
        : v0(a), v1(b), v2(c), n0(na), n1(nb), n2(nc), material(m), smoothNormals(smooth) {
        // Calcular normal geométrica (para usar como fallback o para flat shading)
        Vector3 e1 = v1 - v0;
        Vector3 e2 = v2 - v0;
        Vector3 cross = e1.cross(e2);
        float len = cross.length();
        if (len > 1e-8f) {
            faceNormal = (cross / len).toDirection();
        } else {
            faceNormal = Direction(0.0f, 1.0f, 0.0f);
        }
    }

    bool intersect(Ray& ray, float& t) const override;
    Material getMaterial() const override { return material; }
    Direction getPointNormal(Point p) const override;
    bool emitsLight() const override { return false; }

    AABB boundingBox() const override;
};
