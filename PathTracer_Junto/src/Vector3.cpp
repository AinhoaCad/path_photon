#include "Vector3.hpp"
#include "Direction.hpp" 
#include "Point.hpp"

// Constructores
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

// Conversión a Point y Direction

Point Vector3::toPoint() const {
    return Point(x, y, z);
}

Direction Vector3::toDirection() const {
    return Direction(x, y, z);
}

// Operadores básicos
Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}
Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}
Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}
Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x; y += other.y; z += other.z;
    return *this;
}
Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x; y -= other.y; z -= other.z;
    return *this;
}
Vector3& Vector3::operator*=(float scalar) {
    x *= scalar; y *= scalar; z *= scalar;
    return *this;
}
Vector3& Vector3::operator/=(float scalar) {
    x /= scalar; y /= scalar; z /= scalar;
    return *this;
}

// Comparación
bool Vector3::operator==(const Vector3& other) const {
    return x == other.x && y == other.y && z == other.z;
}

// Álgebra vectorial
float Vector3::dotProduct(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}
Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}
float Vector3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}
Vector3 Vector3::normalized() const {
    float len = length();
    return len > 0 ? *this / len : Vector3();
}
Vector3 Vector3::abs() const {
    return Vector3(std::fabs(x), std::fabs(y), std::fabs(z));
}
Vector3 Vector3::refract(const Vector3& normal, float eta) const {
    // Based on the common refract implementation that handles entering/exiting
    Vector3 N = normal;
    float cosI = -this->dotProduct(N);
    float etaRatio = eta; // eta is expected to be ni/nt from the caller

    // If cosI < 0, the incident vector is coming from the other side -> flip
    if (cosI < 0.0f) {
        cosI = -cosI;
        N = N * -1.0f;
        // invert eta ratio when flipping (now ni/nt becomes nt/ni)
        if (etaRatio != 0.0f) etaRatio = 1.0f / etaRatio;
    }

    float sinT2 = etaRatio * etaRatio * (1.0f - cosI * cosI);
    if (sinT2 > 1.0f) return Vector3(0, 0, 0); // Total internal reflection
    float cosT = std::sqrt(std::max(0.0f, 1.0f - sinT2));
    return (*this) * etaRatio + N * (etaRatio * cosI - cosT);
}

// Funciones utilitarias
Vector3 Vector3::min(const Vector3& v1, const Vector3& v2) {
    return Vector3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}
Vector3 Vector3::max(const Vector3& v1, const Vector3& v2) {
    return Vector3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}
int Vector3::maxDimension(const Vector3& v) {
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}
float Vector3::minComponent(const Vector3& v) {
    return std::min({v.x, v.y, v.z});
}
float Vector3::maxComponent(const Vector3& v) {
    return std::max({v.x, v.y, v.z});
}

// Salida por consola
std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
