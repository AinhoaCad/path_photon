#include "point.hpp"
// Point class method definitions
Point::Point(float x, float y, float z) {
    c[0] = x;
    c[1] = y;
    c[2] = z;
}

Point::Point(array<float, 4> v){
    c[0] = v[0];
    c[1] = v[1];
    c[2] = v[2];
}

ostream& operator<<(ostream& os, const Point& P) {
    os << "(" << P.c[0] << ", " << P.c[1] << ", " << P.c[2] << ")";  
    return os;
}

Point operator*(const Point& P, float r) {
    return Point(r * P.c[0], r * P.c[1], r * P.c[2]);
}

Point operator/(const Point& P, float r) {
    return Point(P.c[0] / r, P.c[1] / r, P.c[2] / r);
}

Direction Point::operator-(const Point& P) {
    return Direction( c[0]-P.c[0], c[1]-P.c[1] ,  c[2]-P.c[2] );
}

array<float, 4> Point::homCoord() const{
    array<float, 4> c={c[0],c[1],c[2],1.0f};
    return c;
}

float Point::operator[] (int coord) const {
    return c[coord];
}

Point Point::operator+(const Direction &P)
{
    return Point(c[0]+P[0], c[1]+P[1] ,  c[2]+P[2] );
}
