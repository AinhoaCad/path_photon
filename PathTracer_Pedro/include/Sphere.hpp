#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Material.hpp"

class Sphere {
public:
    // Constructor
    Sphere(const Vector3& center, float radius, const Material& material);

    // Método para calcular la intersección de un rayo con la esfera
    bool intersect(const Ray& ray, float& t) const;

    // Getter para el centro y el radio de la esfera
    Vector3 center() const;
    float radius() const;

    Vector3 getIntersectionPoint(const Ray& rayo, float t) const;
    Vector3 getNormal(const Ray& rayo, float t) const;
    // Getter para el material
    Material getMaterial() const;

private:
    Vector3 center_;  // Centro de la esfera
    float radius_;    // Radio de la esfera
    Material material_;
};

#endif
