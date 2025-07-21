#include "Point.hpp"

Point::Point() : x(0), y(0), z(0) {}
Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}
Point::Point(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

Vector3 Point::toVector() const {
    return Vector3(x, y, z);
}

Point Point::operator+(const Vector3& v) const {
    return Point(x + v.x, y + v.y, z + v.z);
}
Point Point::operator-(const Vector3& v) const {
    return Point(x - v.x, y - v.y, z - v.z);
}
Vector3 Point::operator-(const Point& p) const {
    return Vector3(x - p.x, y - p.y, z - p.z);
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "Point(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}
