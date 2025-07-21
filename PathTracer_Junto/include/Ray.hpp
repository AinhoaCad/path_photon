#ifndef RAY_HPP
#define RAY_HPP

#include "Vector3.hpp"

class Ray {
public:
    // Constructores
    Ray();  // Constructor por defecto
    Ray(const Vector3& origin, const Vector3& direction);  // Constructor con origen y dirección

    // Métodos principales
    Vector3 origin() const;  // Obtiene el origen del rayo
    Vector3 direction() const;  // Obtiene la dirección del rayo
    Vector3 pointAtParameter(float t) const;  // Calcula el punto del rayo para un parámetro t

    // Sobrecarga de operadores
    Ray operator+(const Vector3& offset) const;  // Desplaza el rayo
    Ray operator-(const Vector3& offset) const;  // Desplaza el rayo en dirección opuesta

private:
    Vector3 origin_;  // Origen del rayo
    Vector3 direction_;  // Dirección del rayo
};

#endif