#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstddef>

//#include "Point.hpp"
//#include "Direction.hpp"

class Point;  
class Direction;  


class Vector3 {
public:
    float x, y, z;

    // Constructores
    Vector3();
    Vector3(float x, float y, float z);


    // Conversión a Point y Direction
    Point toPoint() const;
    Direction toDirection() const;

    // Operadores básicos
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    bool operator==(const Vector3& other) const;

    // Acceso de LECTURA
    const float& operator[](size_t i) const;

    // Acceso de ESCRITURA
    float& operator[](size_t i);

    // Álgebra vectorial
    float dotProduct(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float length() const;
    Vector3 normalized() const;
    Vector3 abs() const;
    Vector3 refract(const Vector3& normal, float eta) const;

    // Funciones utilitarias
    static Vector3 min(const Vector3& v1, const Vector3& v2);
    static Vector3 max(const Vector3& v1, const Vector3& v2);
    static int maxDimension(const Vector3& v);
    static float minComponent(const Vector3& v);
    static float maxComponent(const Vector3& v);

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
};
