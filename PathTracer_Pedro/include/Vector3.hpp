#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>
#include <iostream>
#include <algorithm>

class Vector3 {
public:
    // Atributos
    float x, y, z;

    // Constructores
    Vector3();
    Vector3(float x, float y, float z);

    // Operadores básicos
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Operador ==
    bool operator==(const Vector3& other) const;

    // Métodos para el álgebra vectorial
    float dotProduct(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float length() const;
    Vector3 normalized() const;

    // Nuevas funcionalidades añadidas
    static Vector3 min(const Vector3& v1, const Vector3& v2);
    static Vector3 max(const Vector3& v1, const Vector3& v2);
    static int maxDimension(const Vector3& v);
    static float minComponent(const Vector3& v);
    static float maxComponent(const Vector3& v);
    Vector3 abs() const;

    Vector3 refract(const Vector3& normal, float eta) const;

    // Sobrecarga de salida
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
};

#endif // VECTOR3_HPP
