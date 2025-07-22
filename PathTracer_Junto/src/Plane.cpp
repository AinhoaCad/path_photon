#include "Plane.hpp"
#include <cmath>


// Constructor
Plane::Plane(const Vector3& normal, float d, const Material& material, bool emision)
    : normal_(normal.normalized()), d_(d), material_(material), emision_(emision) {}

// Intersección rayo-plano
bool Plane::intersect(Ray& ray, float& t) const {
    float denom = normal_.dotProduct(ray.direction());
    if (std::fabs(denom) < 1e-6) return false; // Rayo paralelo al plano

    float numerador = - (normal_.dotProduct(ray.origin()) + d_);
    t = numerador / denom;

    return t > 1e-4; // evitar autointersección o planos demasiado cercanos
}

// Getters modernos
Vector3 Plane::normal() const {
    return normal_;
}

float Plane::d() const {
    return d_;
}

Material Plane::getMaterial() const {
    return material_;
}

Vector3 Plane::getIntersectionPoint(const Ray& ray, float t) const {
    return ray.origin() + ray.direction() * t;
}

Vector3 Plane::getNormal(const Ray& ray, float t) const {
    return normal_;
}

// Métodos compatibles (heredados)
Direction Plane::getPointNormal(Point p) const {
    return normal_;
}


bool Plane::getEmision() const {
    return emision_;
}

// Setters de compatibilidad
void Plane::setDistancia(float d) {
    d_ = d;
}

void Plane::setNormal(const Vector3& n) {
    normal_ = n.normalized();
}

void Plane::setMaterial(const Material& material) {
    material_ = material;
}

