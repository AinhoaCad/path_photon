#include <iostream>
#include "matrix.h"

class Point{
    float p[3];
public:
    Point(float x,float y, float z) {
        p[0] = x; p[1] = y; p[2] = z;
    }
    Point(const Matrix& m) {
        p[0] = m(0,0); p[1] = m(1,0); p[2] = m(2,0);
    }
    float getX() const { return p[0]; }
    float getY() const { return p[1]; }
    float getZ() const { return p[2]; }
    
    Matrix getHomMatrix() const {
        Matrix m(4,1);
        m(0,0) = p[0];
        m(1,0) = p[1];
        m(2,0) = p[2];
        m(3,0) = 1;
        return m;
    }
    // Sobrecarga del operador << para imprimir el objeto Point
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << "[" << p.getX() << ", " << p.getY() << ", " << p.getZ() << "]";
        return os;
    }
};