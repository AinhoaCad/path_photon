#include "ray.hpp"

Ray::Ray(Direction _d,Point _p) : d(_d), p(_p),RefractionIndex(1.0) {

}

Ray::Ray(Direction _d, Point _p, float _RefractionIndex): d(_d), p(_p),RefractionIndex(_RefractionIndex)
{
}

Direction Ray::getDirection()
{
    return d;
}

Point Ray::getPoint()
{
    return p;
}

float Ray::getRefractionIndex()
{
    return RefractionIndex;
}

void Ray::setRefractionIndex(float n)
{
    RefractionIndex=n;
}
