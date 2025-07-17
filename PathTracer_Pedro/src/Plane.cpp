#include "Plane.hpp"
#include "Material.hpp"
#include <cmath>

// Constructor que inicializa el plano con su normal y distancia
Plane::Plane(const Vector3& normal, float d,  const Material& material) : normal_(normal.normalized()), d_(d), material_(material) {}

// Método para calcular la intersección de un rayo con el plano
bool Plane::intersect(const Ray& ray, float& t) const {
    // Producto escalar entre la dirección del rayo y la normal del plano
    float denom = normal_.dotProduct(ray.direction());

    // Si el rayo es paralelo al plano (denominador cercano a 0), no hay intersección
    if (std::fabs(denom) < 1e-6) {
        return false;
    }

    // Calculamos el parámetro t de la intersección
    float num = normal_.dotProduct(ray.origin()) + d_;
    t = -(num / denom);

    // La intersección es válida si t > 0 (delante del origen del rayo)
    return t > 0;
}

// Getter para la normal del plano
Vector3 Plane::normal() const {
    return normal_;
}

// Getter para la distancia al origen
float Plane::d() const {
    return d_;
}

// Métodos para obtener el punto de intersección y la normal
Vector3 Plane::getIntersectionPoint(const Ray& rayo, float t) const {
    return rayo.origin() + rayo.direction() * t;
}

Vector3 Plane::getNormal() const {
    return normal_;
}

// Getter para el material
Material Plane::getMaterial() const {
    return material_;
}