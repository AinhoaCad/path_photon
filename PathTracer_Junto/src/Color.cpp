#include "Color.hpp"

Color Color::operator+(const Color& other) const {
    return Color(R + other.R, G + other.G, B + other.B);
}

Color Color::operator-(const Color& other) const {
    return Color(R - other.R, G - other.G, B - other.B);
}

Color Color::operator*(float scalar) const {
    return Color(R * scalar, G * scalar, B * scalar);
}

Color Color::operator*(const Color& other) const {
    return Color(R * other.R, G * other.G, B * other.B);
}

Color& Color::operator+=(const Color& other) {
    R += other.R;
    G += other.G;
    B += other.B;
    return *this;
}

Color& Color::operator-=(const Color& other) {
    R -= other.R;
    G -= other.G;
    B -= other.B;
    return *this;
}

Color& Color::operator*=(float scalar) {
    R *= scalar;
    G *= scalar;
    B *= scalar;
    return *this;
}

Color Color::operator/(float scalar) const {
    // escalar no sea cero para evitar la división por cero
    if (scalar == 0) {
        //color negro o un color especial de error, según tus necesidades
        return Color(0, 0, 0);
    }
    return Color(R / scalar, G / scalar, B / scalar);
}

//Pasar a Pixel
Pixel Color::toPixel() const {
    return Pixel{R, G, B};
}


