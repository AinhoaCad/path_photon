#ifndef PLANE_HPP
#define PLANE_HPP

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Material.hpp"

class Plane {
public:
    // Constructor que inicializa el plano con su normal y su distancia desde el origen
    Plane(const Vector3& normal, float d, const Material& material);

    // Método para calcular la intersección de un rayo con el plano
    bool intersect(const Ray& ray, float& t) const;

    // Getters
    Vector3 normal() const;
    float d() const;

    Vector3 getIntersectionPoint(const Ray& rayo, float t) const;
    Vector3 getNormal() const;
    Material getMaterial() const;

private:
    Vector3 normal_; // Vector normal al plano
    float d_;        // Distancia desde el origen al plano
    Material material_;  // Material del plano
};

#endif // PLANE_HPP
