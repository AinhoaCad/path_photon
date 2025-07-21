#include "Direction.hpp"
#include <cmath>

Direction::Direction() : x(0), y(0), z(0) {}
Direction::Direction(float x, float y, float z) : x(x), y(y), z(z) {}
Direction::Direction(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

Vector3 Direction::toVector() const {
    return Vector3(x, y, z);
}

Direction Direction::normalized() const {
    float len = std::sqrt(x*x + y*y + z*z);
    return len > 0 ? Direction(x/len, y/len, z/len) : Direction();
}

Direction Direction::operator*(float scalar) const {
    return Direction(x * scalar, y * scalar, z * scalar);
}
Direction Direction::operator/(float scalar) const {
    return Direction(x / scalar, y / scalar, z / scalar);
}

float Direction::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

std::ostream& operator<<(std::ostream& os, const Direction& d) {
    os << "Direction(" << d.x << ", " << d.y << ", " << d.z << ")";
    return os;
}
