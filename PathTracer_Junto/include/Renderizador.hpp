#pragma once
#include "Ray.hpp"
#include "Primitive.hpp"
#include "Image.hpp"
#include <vector>
#include "Light.hpp"
#include "Plane.hpp"

// Funciones auxiliares para el renderizado


struct Interseccion {
    bool hayInterseccion;
    float t;
    Point punto;
    Direction normal;
    Primitive* primitiva;
};


float randomOffset(float pixelSize);

Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives);

Pixel promedioColores(const std::vector<Pixel>& colores);

Interseccion calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives);

// Comprueba si un punto está en sombra respecto a una luz
bool estaEnSombra(const Point& punto, const Direction& normal, const Light& luz, const std::vector<Primitive*>& primitives);

// Calcula la iluminación total de un punto (difusa + emisiva)
Pixel calcularIluminacion(const Interseccion& inter, const std::vector<Light>& luces, const std::vector<Primitive*>& primitives);