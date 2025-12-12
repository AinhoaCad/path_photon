#ifndef LIGHTSOURCE_HPP
#define LIGHTSOURCE_HPP
#include "point.hpp"
#include "direction.hpp"
#include "matrix.hpp"
#include "ray.hpp"
#include "RGB.hpp"
#include "primitive.hpp"

class LightSource{
    public:
        LightSource();
        virtual RGB getLightIntensity()=0;
        virtual Point getCenter()=0;
        virtual bool isAreaLight()=0;
        virtual bool doesIntersect(Ray r,Intersection& inter)=0;
        virtual RGB getEmision()=0;

};

class PointLight: public LightSource{
    Point o;
    RGB color;
    RGB photons;
    public:
        PointLight(Point _o,RGB _color);
        Point getCenter() override;
        RGB getLightIntensity() override;
        bool isAreaLight() override;
        bool doesIntersect(Ray r,Intersection& inter) override;
        RGB getEmision() override;
        void setPhotons(RGB _photons);
        RGB getPhotons();

};

class AreaLight: public LightSource{
    Primitive* p;
    public:
        AreaLight(Primitive &_p);
        Point getCenter() override;
        bool isAreaLight() override;
        RGB getLightIntensity() override;
        bool doesIntersect(Ray r,Intersection& inter) override;
        RGB getEmision() override;
};
#endif