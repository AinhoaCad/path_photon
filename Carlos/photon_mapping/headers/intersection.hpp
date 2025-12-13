#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP
#include "point.hpp"
#include "direction.hpp"
#include "RGB.hpp"
#include "material.hpp"

class Intersection{
    float distance;
    Point point;
    Direction normal;
    Material material;
    public:
        Intersection();
        Intersection(float d, Point p, Direction n, Material _material);
        float getDistance();
        Direction getNormal();
        Point getPoint();
        RGB getK_d();
        RGB getK_s();
        RGB getK_t();
        RGB getK_e();
        float getRefractionIndex();
        void setDistance(float d);
        void setNormal(Direction n);
        void setPoint(Point p);
        void setK_d(RGB _k_d);
        void setK_s(RGB _k_s);
        void setK_t(RGB _k_t);
        void setK_e(RGB _k_e);
        void setRefractionIndex(float n);
};
#endif