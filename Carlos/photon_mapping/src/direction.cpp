#include "direction.hpp"
// Direction class method definitions
Direction::Direction(float x, float y, float z) {
    c[0] = x; c[1] = y; c[2] = z;
}

Direction::Direction(array<float, 4> h) {
    c[0] = h[0]; c[1] = h[1]; c[2] = h[2];
}

ostream& operator<<(ostream& os, const Direction& D) {
    os << "[" << D.c[0] << ", " << D.c[1] << ", " << D.c[2] << "]";
    return os;
}

Direction operator*(const Direction& D, float r) {
    return Direction(r * D.c[0], r * D.c[1], r * D.c[2]);
}

Direction operator/(const Direction& D,float r) {
    return Direction((float)D.c[0] / r, (float)D.c[1] / r,(float) D.c[2] / r);
}

Direction operator-(const Direction& D1, const Direction& D2) {
    return Direction(D1.c[0] - D2.c[0], D1.c[1] - D2.c[1], D1.c[2] - D2.c[2]);
}

Direction operator+(const Direction& D1, const Direction& D2) {
    return Direction(D1.c[0] + D2.c[0], D1.c[1] + D2.c[1], D1.c[2] + D2.c[2]);
}

bool operator==(const Direction& D1, const Direction& D2) {
    return (D1.c[0] == D2.c[0] && D1.c[1] == D2.c[1] && D1.c[2] == D2.c[2]);
}

float Direction::modulus() const {
    return sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
}

Direction Direction::normalization() const {
    float mod = modulus();
    if (mod == 0) {
        throw runtime_error("Error: no se puede normalizar una dirección con módulo 0.");
    }
    return *this / mod;
}

Direction Direction::cross_product(const Direction& D) const {
    return Direction(c[1] * D.c[2] - c[2] * D.c[1],
                     c[2] * D.c[0] - c[0] * D.c[2],
                     c[0] * D.c[1] - c[1] * D.c[0]);
}

float Direction::dot_product(const Direction& D) const {
    return c[0] * D.c[0] + c[1] * D.c[1] + c[2] * D.c[2];
}

array<float, 4> Direction::homCoord() const{
    return {c[0],c[1],c[2],0.0f};
}

float Direction::operator[] (int coord) const{
    return c[coord];
}