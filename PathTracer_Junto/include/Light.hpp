#pragma once

#include "Point.hpp"
#include "Image.hpp"
#include "Operations.hpp"
#include "Vector3.hpp"
#include <cmath>

class Light {
public:

    // Atributos

    // ----------- Siempre -----------
    Point origen;
    Pixel power;

    // ----------- Solo si es área rectangular -----------
    Vector3 normal;   // Normal del área
    float width = 0;  // Ancho (si ==0, la luz se considera puntual)
    float height = 0; // Alto

    // Constructor por defecto
    Light();

    // Constructor en un punto
    Light(Point origen, Pixel power);

    Light(const Vector3& origenVec, const Pixel& powerPixel);

    // Constructor en un area
    Light(Point centro, const Vector3& normal, float width, float height, const Pixel& powerPixel); // Luz en un área rectangular


    // ----------- Métodos de luz puntual -----------

    // Intensidad desde un punto
    Pixel calcularIntensidadLuz(Point p) const;

    // Luz difusa usando modelo Lambertiano
    Pixel BRDF(Pixel color) const;
    float LambertianCos(Direction normal, Point p) const;

    // Luz total recibida en un punto p con normal y color dados
    Pixel calcularLuz(Point p, Direction normal, Pixel color) const;



    // ----------- Métodos de área -----------

    // Devuelve un punto aleatorio dentro del área rectangular
    Point samplePointOnArea(float xi1, float xi2) const;

    // Intensidad desde un punto muestreado en el área
    Pixel calcularIntensidadLuz(Point p, const Point& lightSample) const;

    // Luz difusa usando un punto muestreado en el área
    Pixel calcularLuzAreaSample(Point p, Direction normal, Pixel color, const Point& lightSample) const;
};
