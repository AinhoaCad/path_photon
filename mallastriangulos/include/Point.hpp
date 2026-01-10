#pragma once

#include "Vector3.hpp"
#include "Matrix.hpp"

class Point {
public:
    float x, y, z;

    // Constructores
    Point();
    Point(float x, float y, float z);
    Point(const Vector3& v); // Conversión desde Vector3

    // Conversión a Vector3
    Vector3 toVector() const;

    // Operador Matriz homogénea
    Matrix getHomMatrix() const;

    // Operaciones con vectores y otros puntos
    Point operator+(const Vector3& v) const;
    Point operator-(const Vector3& v) const;
    Vector3 operator-(const Point& p) const;

    // Utilidades
    friend std::ostream& operator<<(std::ostream& os, const Point& p);

    // Getters
    float getX() const;
    float getY() const;
    float getZ() const;
    
        
};
