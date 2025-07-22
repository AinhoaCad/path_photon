#pragma once

#include "Point.hpp"
#include "Image.hpp"
#include "Operations.hpp"
#include <cmath>

class Light {
public:
    Point origen;
    Pixel power;

    // Constructor
    Light(Point origen, Pixel power);

    Light(const Vector3& origenVec, const Pixel& powerPixel);

    // Métodos
    Pixel calcularIntensidadLuz(Point p) const;
    Pixel BRDF(Pixel color) const;
    float LambertianCos(Direction normal, Point p) const;
    Pixel calcularLuz(Point p, Direction normal, Pixel color) const;
};
