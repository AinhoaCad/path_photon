#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "RGB.hpp"

using namespace std;

class Material {
    RGB k_d;
    RGB k_s;
    RGB k_t;
    RGB k_e;
    float RefractionIndex;

public:
    Material (RGB _k_d,RGB _k_s, RGB _k_t,RGB _k_e, float _RefractionIndex);

    Material ();

    RGB getK_d();
    void setK_d(RGB k);

    RGB getK_s();
    void setK_s(RGB k);

    RGB getK_t();
    void setK_t(RGB k);

    RGB getK_e();
    void setK_e(RGB k);

    float getRefractionIndex();
    void setRefractionIndex(float _n);
    // bool doesIntersect(Ray r, Intersection &intersection) const;
};

#endif