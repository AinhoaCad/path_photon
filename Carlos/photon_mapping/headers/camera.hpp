#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "point.hpp"
#include "direction.hpp"
#include "matrix.hpp"

class Camera{
    Point o;
    Direction u;
    Direction l;
    Direction f;
    public:
        Camera(Point _o,Direction _l,Direction _u,Direction _f);
        Point getOrigin();
        //Matrix change
        Direction getu();
        Direction getl();
        Direction getf();

};
#endif