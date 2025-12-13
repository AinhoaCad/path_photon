#ifndef RAY_HPP
#define RAY_HPP
#include "point.hpp"
#include "direction.hpp"

class Ray{
    Point p;
    Direction d;
    float RefractionIndex;
    public:
        Ray(Direction _d, Point _p);
        Ray(Direction _d, Point _p,float _RefractionIndex);
        Direction getDirection();
        Point getPoint();
        float getRefractionIndex();
        void setRefractionIndex(float n);
};
#endif