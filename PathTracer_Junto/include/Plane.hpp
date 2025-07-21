#pragma once

#include "Material.hpp"
#include "Primitive.hpp"

class Plane : public Primitive {
public:
    // Constructor
    Plane(const Vector3& normal, float d, const Material& material, bool emision);

    // Métodos de intersección
    bool intersect(Ray& ray, float& t) const override;

    // Getters
    Vector3 normal() const;
    float d() const;
    Vector3 getIntersectionPoint(const Ray& ray, float t) const;
    Vector3 getNormal(const Ray& ray, float t) const;
    Direction getPointNormal(Point p) const override;

    Material getMaterial() const;
    Pixel getColor() const override; // Proxy desde material
    bool getEmision() const override;

    // Setters (compatibilidad)
    void setDistancia(float d);
    void setNormal(const Vector3& n);
    void setColor(const Pixel& col);

protected:
    Vector3 normal_;
    float d_;
    Material material_;
    bool emision_;
};
