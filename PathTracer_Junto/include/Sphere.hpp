#pragma once

#include "Material.hpp"
#include "Primitive.hpp"

class Sphere : public Primitive {
public:
    // Constructor
    Sphere(const Vector3& center, float radius, const Material& material, bool emision);

    // Métodos de intersección
    bool intersect(Ray& ray, float& t) const override;

    // Getters
    Vector3 center() const;
    float radius() const;
    Vector3 getCentro() const; // Compatibilidad con versión anterior
    Vector3 getIntersectionPoint(const Ray& ray, float t) const;
    Vector3 getNormal(const Ray& ray, float t) const;
    Direction getPointNormal(Point p) const override;

    Material getMaterial() const override;
    bool getEmision() const;

    // Setters (compatibilidad)
    void setCentro(const Vector3& c);
    void setRadio(float r);
    void setMaterial(const Material& material);

protected:
    Vector3 center_;
    float radius_;
    Material material_;
    bool emision_;
};

