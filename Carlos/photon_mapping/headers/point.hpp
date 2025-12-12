#ifndef POINT_HPP
#define POINT_HPP

#include <iostream>
#include <cmath>
#include "direction.hpp"

using namespace std;

class Point {
    float c[3];
public:
    // Constructor
    Point(float x, float y, float z);
    Point(array<float, 4> v);

    // Friend functions for stream insertion (<<) and arithmetic operators 
    friend ostream& operator<<(ostream& os, const Point& P);
    float operator[] (int coord) const;
    friend Point operator*(const Point& P, float r);
    friend Point operator/(const Point& P, float r);
    Point operator+(const Direction& P);

    // Subtraction operator between two points, returning a Direction
    Direction operator-(const Point& P);

    array<float, 4> homCoord() const;
};

#endif 