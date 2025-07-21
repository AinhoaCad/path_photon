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
    Pixel calcularIntensidadLuz(Point p);
    Pixel BRDF(Pixel color);
    float LambertianCos(Direction normal, Point p);
    Pixel calcularLuz(Point p, Direction normal, Pixel color);
};
