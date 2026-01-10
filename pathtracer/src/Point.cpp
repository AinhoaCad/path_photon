#include "Point.hpp"

Point::Point() : x(0), y(0), z(0) {}
Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}
Point::Point(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

Vector3 Point::toVector() const {
    return Vector3(x, y, z);
}

Matrix Point::getHomMatrix() const {
        Matrix m(4,1);
        m(0,0) = x;
        m(1,0) = y;
        m(2,0) = z;
        m(3,0) = 1;
        return m;
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

float Point::getX() const { return x; }

float Point::getY() const { return y; }

float Point::getZ() const { return z; }
