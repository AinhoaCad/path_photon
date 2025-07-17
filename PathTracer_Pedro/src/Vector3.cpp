#include "Vector3.hpp"




// Constructores
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

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
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// Métodos para el álgebra vectorial
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

// Nuevas funcionalidades añadidas
Vector3 Vector3::abs() const {
    return Vector3(std::fabs(x), std::fabs(y), std::fabs(z));
}

// Métodos de la clase Vector3
Vector3 Vector3::refract(const Vector3& normal, float eta) const {
    float cosI = -this->dotProduct(normal);  // Ángulo entre el vector de dirección y la normal
    float sinT2 = eta * eta * (1.0f - cosI * cosI);  // Cálculo de sin²(θ_t)

    // Si la refracción no ocurre (cuando el ángulo es demasiado grande para que la luz se refracte)
    if (sinT2 > 1.0f) {
        return Vector3(0, 0, 0);  // La refracción no es posible
    }

    float cosT = std::sqrt(1.0f - sinT2);  // Cálculo de cos(θ_t)
    return (*this) * eta + normal * (eta * cosI - cosT);  // Fórmula de Snell para el vector refractado
}



Vector3 Vector3::min(const Vector3& v1, const Vector3& v2) {
    return Vector3(
        std::min(v1.x, v2.x),
        std::min(v1.y, v2.y),
        std::min(v1.z, v2.z)
    );
}

Vector3 Vector3::max(const Vector3& v1, const Vector3& v2) {
    return Vector3(
        std::max(v1.x, v2.x),
        std::max(v1.y, v2.y),
        std::max(v1.z, v2.z)
    );
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

// Sobrecarga de salida
std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Operador ==
bool Vector3::operator==(const Vector3& other) const {
    return x == other.x && y == other.y && z == other.z;
}
