#include "lightSource.hpp"

RGB LightSource::getLightIntensity() { return RGB(); }
Point LightSource::getCenter() { return Point(0,0,0); }
bool LightSource::isAreaLight() { return false; }
bool LightSource::doesIntersect(Ray r,Intersection& inter) { return false; }
RGB LightSource::getEmision() { return RGB(); }

LightSource::LightSource()
{
    
}
PointLight::PointLight(Point _o, RGB _color): o(_o), color(_color)
{
    
}
Point PointLight::getCenter()
{
    return o;
}

RGB PointLight::getLightIntensity()
{
    return color;
}

bool PointLight::isAreaLight()
{
    return false;
}

bool PointLight::doesIntersect(Ray r,Intersection& inter)
{
    Point endofRay = r.getPoint()+r.getDirection();
    return (endofRay[0]==o[0] && endofRay[1]==o[1] && endofRay[2]==o[2]);
}

RGB PointLight::getEmision()
{
    return RGB(0,0,0);
}

void PointLight::setPhotons(RGB _photons)
{
    photons=_photons;
}

RGB PointLight::getPhotons()
{
    return photons;
}

bool AreaLight::doesIntersect(Ray r,Intersection& inter)
{
    return p->doesIntersect(r,inter);
}

RGB AreaLight::getEmision()
{
    return p->getK_e();
}

AreaLight::AreaLight(Primitive &_p):p(&_p)
{
}

Point AreaLight::getCenter()
{
    return Point(0,0,0);
}

bool AreaLight::isAreaLight()
{
    return true;
}

RGB AreaLight::getLightIntensity()
{
    return p->getK_e();
}
