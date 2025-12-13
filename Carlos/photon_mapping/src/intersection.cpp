#include "intersection.hpp"

Intersection::Intersection(): distance(std::numeric_limits<float>::infinity()), point(0,0,0),normal(0,0,0),material(RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1.0)
{
}

Intersection::Intersection(float d, Point p, Direction n, Material _material) : distance(d), point(p), normal(n), material(_material)
{
}

float Intersection::getDistance()
{
    return distance;
}

Direction Intersection::getNormal()
{
    return normal;
}

Point Intersection::getPoint() 
{
    return point;
}

RGB Intersection::getK_d() 
{
    return material.getK_d();
}

RGB Intersection::getK_s() 
{
    return material.getK_s();
}

RGB Intersection::getK_t() 
{
    return material.getK_t();
}

RGB Intersection::getK_e() 
{
    return material.getK_e();
}

float Intersection::getRefractionIndex()
{
    return material.getRefractionIndex();
}

void Intersection::setDistance(float d)
{
    distance=d;
}

void Intersection::setNormal(Direction n)
{
    normal=n;
}

void Intersection::setPoint(Point p)
{
    point=p;
}

void Intersection::setK_d(RGB _k_d)
{
    material.setK_d(_k_d);
}

void Intersection::setK_s(RGB _k_s)
{
    material.setK_s(_k_s);
}

void Intersection::setK_t(RGB _k_t)
{
    material.setK_t(_k_t);
}

void Intersection::setK_e(RGB _k_e)
{
    material.setK_e(_k_e);
}

void Intersection::setRefractionIndex(float n)
{
    material.setRefractionIndex(n);
}
