#include "material.hpp"
#include <iostream>

Material::Material(RGB _k_d,RGB _k_s,RGB _k_t,RGB _k_e,float _RefractionIndex):k_d(_k_d),k_s(_k_s),k_t(_k_t),k_e(_k_e),RefractionIndex(_RefractionIndex)
{
}

Material::Material()
    : k_d(0.82, 0.6, 0.28), k_s(0, 0, 0), k_t(0, 0, 0), k_e(0, 0, 0), RefractionIndex(1.0) {}


RGB Material::getK_d ()
{
    return k_d;
}

void Material::setK_d(RGB k)
{
    k_d=k;
}

RGB Material::getK_s()
{
    return k_s;
}

void Material::setK_s(RGB k)
{
    k_s=k;
}

RGB Material::getK_t()
{
    return k_t;
}

void Material::setK_t(RGB k)
{
    k_t=k;
}

RGB Material::getK_e()
{
    return k_e;
}

void Material::setK_e(RGB k)
{
    k_e=k;
}

float Material::getRefractionIndex()
{
    return RefractionIndex;
}

void Material::setRefractionIndex(float _n)
{
    RefractionIndex=_n;
}
