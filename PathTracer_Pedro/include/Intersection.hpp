#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "Vector3.hpp"
#include "Material.hpp"

class Intersection {
public:
    Vector3 punto;       // Punto de intersección
    Vector3 normal;      // Normal en el punto de intersección
    Material material;   // Material del objeto intersectado
    float t;             // Distancia del rayo a la intersección

    // Constructor por defecto
    Intersection() : punto(), normal(), material(), t(0) {}

    // Constructor parametrizado
    Intersection(const Vector3& p, const Vector3& n, const Material& m, float tVal)
        : punto(p), normal(n), material(m), t(tVal) {}
};

#endif // INTERSECTION_HPP
