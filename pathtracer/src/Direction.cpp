#include "Direction.hpp"
#include <cmath>

// Constructores
Direction::Direction() : x(0), y(0), z(0) {}
Direction::Direction(float x, float y, float z) : x(x), y(y), z(z) {}
Direction::Direction(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
Direction::Direction(const Matrix& m) {
    x = m(0, 0);
    y = m(1, 0);
    z = m(2, 0);
}

// Conversión a Vector3
Vector3 Direction::toVector() const {
    return Vector3(x, y, z);
}

// Conversión a matriz homogénea
Matrix Direction::getHomMatrix() const {
    Matrix m(4, 1);
    m(0, 0) = x;
    m(1, 0) = y;
    m(2, 0) = z;
    m(3, 0) = 0;
    return m;
}

// Normalización
Direction Direction::normalized() const {
    float len = length();
    if (len == 0) return Direction(0, 0, 0);
    return *this / len;
}

float Direction::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

// Escalado
Direction Direction::operator*(float scalar) const {
    return Direction(x * scalar, y * scalar, z * scalar);
}

Direction Direction::operator/(float scalar) const {
    if (scalar == 0) return Direction(0, 0, 0);
    return Direction(x / scalar, y / scalar, z / scalar);
}

// Getters
float Direction::getX() const { return x; }
float Direction::getY() const { return y; }
float Direction::getZ() const { return z; }

// Output
std::ostream& operator<<(std::ostream& os, const Direction& d) {
    os << "Direction(" << d.x << ", " << d.y << ", " << d.z << ")";
    return os;
}
