#ifndef DIRECTION_HPP
#define DIRECTION_HPP

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <array>

using namespace std;

class Direction {
    float c[3];
public:
    // Constructor
    Direction(float x, float y, float z);

    // Constructor por defecto
    Direction() : c{0.0f, 0.0f, 0.0f} {} // Inicializa a (0, 0, 0)

    Direction(array<float, 4> h);

    // Friend function for stream insertion (<<)
    friend ostream& operator<<(ostream& os, const Direction& D);

    // Arithmetic operators
    friend Direction operator*(const Direction& D, float r);
    friend Direction operator/(const Direction& D, float r);
    friend Direction operator-(const Direction& D1, const Direction& D2);
    friend Direction operator+(const Direction& D1, const Direction& D2);
    friend bool operator==(const Direction& D1, const Direction& D2);

    // Vector operations
    float modulus() const;
    Direction normalization() const;
    Direction cross_product(const Direction& D) const;
    float dot_product(const Direction& D) const;
    array<float, 4> homCoord() const;
    float operator[] (int coord) const;
};

#endif 