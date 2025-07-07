#pragma once

#include <iostream>
#include "matrix.h"

class Direction {
    float d[3];
public:
    Direction(float x,float y, float z) {
        d[0] = x; d[1] = y; d[2] = z;
    }
    Direction(const Matrix& m) {
        d[0] = m(0,0);
        d[1] = m(1,0);
        d[2] = m(2,0);
    }
    float getX() const { return d[0]; }
    float getY() const { return d[1]; }
    float getZ() const { return d[2]; }
    Matrix getHomMatrix() const {
        Matrix m(4,1);
        m(0,0) = d[0];
        m(1,0) = d[1];
        m(2,0) = d[2];
        m(3,0) = 0;
        return m;
    }
    
    // Sobrecarga del operador << para imprimir el objeto Direction
    friend std::ostream& operator<<(std::ostream& os, const Direction& d) {
        os << "[" << d.getX() << ", " << d.getY() << ", " << d.getZ() << "]";
        return os;
    }
};