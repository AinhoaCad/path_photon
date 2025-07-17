#include "Sphere.hpp"
#include "Vector3.hpp"
#include <cmath>

// Constructor que inicializa el centro y el radio de la esfera
Sphere::Sphere(const Vector3& center, float radius, const Material& material) : center_(center), radius_(radius), material_(material) {}

// Método para calcular la intersección de un rayo con la esfera
bool Sphere::intersect(const Ray& ray, float& t) const {
    // Calculamos la dirección desde el origen del rayo al centro de la esfera
    Vector3 oc = ray.origin() - center_;

    // Usamos la fórmula cuadrática para encontrar la intersección
    float a = ray.direction().dotProduct(ray.direction());
    float b = 2.0f * oc.dotProduct(ray.direction());
    float c = oc.dotProduct(oc) - radius_ * radius_;

    // Calculamos el discriminante de la ecuación cuadrática
    float discriminant = b * b - 4.0f * a * c;

    // Si el discriminante es negativo, no hay intersección
    if (discriminant < 0) {
        return false;
    }

    // Calculamos las dos posibles soluciones (intersecciones)
    float sqrtDiscriminant = std::sqrt(discriminant);
    float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

    // Seleccionamos la intersección más cercana (si t0 es mayor que 0)
    t = (t0 > 0) ? t0 : t1;

    return t > 0;  // Si t es mayor que 0, hay una intersección
}

// Getter para el centro de la esfera
Vector3 Sphere::center() const {
    return center_;
}

// Getter para el radio de la esfera
float Sphere::radius() const {
    return radius_;
}

Material Sphere::getMaterial() const {
    return material_;
}

Vector3 Sphere::getIntersectionPoint(const Ray& rayo, float t) const {
    return rayo.origin() + rayo.direction() * t;
}

Vector3 Sphere::getNormal(const Ray& rayo, float t) const {
    Vector3 puntoInterseccion = getIntersectionPoint(rayo, t);
    return (puntoInterseccion - center()).normalized();
}
