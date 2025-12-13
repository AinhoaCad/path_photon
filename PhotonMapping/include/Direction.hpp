#pragma once

#include "Vector3.hpp"
#include "Matrix.hpp"

class Direction {
public:
    float x, y, z;

    // Constructores
    Direction(); // Por defecto
    Direction(float x, float y, float z);
    Direction(const Vector3& v);
    Direction(const Matrix& m); // Nuevo constructor desde matriz

    // Conversión
    Vector3 toVector() const;
    Matrix getHomMatrix() const; // Devuelve matriz homogénea

    // Normalización y longitud
    Direction normalized() const;
    float length() const;

    // Operadores escalares
    Direction operator*(float scalar) const;
    Direction operator/(float scalar) const;

    // Getters para acceso directo
    float getX() const;
    float getY() const;
    float getZ() const;

    // Debug/stream
    friend std::ostream& operator<<(std::ostream& os, const Direction& d);

};
