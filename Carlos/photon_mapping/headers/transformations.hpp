#ifndef TRANSFORMATIONS_HPP
#define TRANSFORMATIONS_HPP

#include <iostream>
#include <stdexcept>
#include <math.h>
#include "point.hpp"
#include "direction.hpp"
#include "matrix.hpp"

using namespace std;

class Transformation{
public:
    static Matrix Transalation(float tx,float ty,float tz);
    static Matrix Scale(float sx,float sy,float sz);
    static Matrix Rotation(char axis, float inclination);
    static Matrix ChangeBasis(const Direction& u,const Direction& v, const Direction& w, const Point& o);
    static Matrix InverseChangeBasis(const Matrix& basis);
};

#endif