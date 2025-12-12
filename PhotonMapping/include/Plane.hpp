#pragma once

#include "Material.hpp"
#include "Primitive.hpp"
#include "Light.hpp"

class Plane : public Primitive {
public:
    // Constructor de plano sin emisión
    Plane(const Vector3& normal, float d, const Material& material);

    // Plano con luz de área
    Plane(const Vector3& normal, float d, const Material& material, const Light& areaLight);

    // Métodos de intersección
    bool intersect(Ray& ray, float& t) const override;

    // Getters
    Vector3 normal() const;
    float d() const;
    Vector3 getIntersectionPoint(const Ray& ray, float t) const;
    Vector3 getNormal(const Ray& ray, float t) const;
    Direction getPointNormal(Point p) const override;

    Material getMaterial() const override;
    

    // Luz asociada
    Light getAreaLight() const;
    bool emitsLight() const override; // devuelve true si el plano emite luz

    // Setters (compatibilidad)
    void setDistancia(float d);
    void setNormal(const Vector3& n);
    void setMaterial(const Material& material);
    void setAreaLight(const Light& light);

protected:
    Vector3 normal_;
    float d_;
    Material material_;
    

    bool emision_ = false; // true si hay luz de área
    Light areaLight_;      // solo válido si emision_ == true
};
