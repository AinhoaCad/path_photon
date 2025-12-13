#include "Ray.hpp"

// Constructor por defecto
Ray::Ray() : origin_(Vector3(0, 0, 0)), direction_(Vector3(0, 0, 1)) {}

// Constructor con origen y dirección
Ray::Ray(const Vector3& origin, const Vector3& direction)
    : origin_(origin), direction_(direction) {}

// Obtiene el origen del rayo
Vector3 Ray::origin() const {
    return origin_;
}

// Obtiene la dirección del rayo
Vector3 Ray::direction() const {
    return direction_;
}

// Calcula el punto del rayo para un parámetro t (punto en la trayectoria del rayo)
Vector3 Ray::pointAtParameter(float t) const {
    return origin_ + direction_ * t;
}

// Sobrecarga de operadores para desplazamiento del rayo
Ray Ray::operator+(const Vector3& offset) const {
    return Ray(origin_ + offset, direction_);
}

Ray Ray::operator-(const Vector3& offset) const {
    return Ray(origin_ - offset, direction_);
}