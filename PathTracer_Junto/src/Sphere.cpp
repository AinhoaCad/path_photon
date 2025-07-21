#include "Sphere.hpp"
#include <cmath>

// Constructor
Sphere::Sphere(const Vector3& center, float radius, const Material& material, bool emision)
    : center_(center), radius_(radius), material_(material), emision_(emision) {}

// Intersección rayo-esfera
bool Sphere::intersect(const Ray& ray, float& t) const {
    Vector3 oc = ray.origin() - center_;
    float a = ray.direction().dotProduct(ray.direction());
    float b = 2.0f * oc.dotProduct(ray.direction());
    float c = oc.dotProduct(oc) - radius_ * radius_;
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0) return false;

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t1_ = (-b + sqrtDiscriminant) / (2.0f * a);

    if (t0 > 0) {
        t = t0;
    } else if (t1_ > 0) {
        t = t1_;
    } else {
        return false;
    }

    return true;
}

// Getters modernos
Vector3 Sphere::center() const {
    return center_;
}

float Sphere::radius() const {
    return radius_;
}

Material Sphere::getMaterial() const {
    return material_;
}

Vector3 Sphere::getIntersectionPoint(const Ray& ray, float t) const {
    return ray.origin() + ray.direction() * t;
}

Vector3 Sphere::getNormal(const Ray& ray, float t) const {
    Vector3 p = getIntersectionPoint(ray, t);
    return (p - center_).normalized();
}

// Métodos compatibles (heredados)
Vector3 Sphere::getCentro() const {
    return center_;
}

Direction Sphere::getPointNormal(Point p) const {
    Operations op;
    return op.norm(op.sub(p, center_));
}

Pixel Sphere::getColor() const {
    return material_.color;
}

bool Sphere::getEmision() const {
    return emision_;
}

// Setters de compatibilidad
void Sphere::setCentro(const Vector3& c) {
    center_ = c;
}

void Sphere::setRadio(float r) {
    radius_ = r;
}

void Sphere::setColor(const Pixel& col) {
    material_.color = col;
}
