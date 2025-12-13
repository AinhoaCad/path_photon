#pragma once

#include <iostream>
#include <math.h>
#include "Point.hpp"
#include "Direction.hpp"
#include <cmath>
#include "Vector3.hpp" // Asegúrate de incluir Vector3 si aún no lo has hecho

class Operations {
public:
    // SUMAS
    static Point add(Point p, Direction d);
    static Direction add(Direction d1, Direction d2);

    // RESTAS
    static Direction sub(Point p1, Point p2);
    static Direction sub(Direction d1, Direction d2);
    static Point sub(Point p, Direction d);

    // MULTIPLICACIONES
    static Direction mult(Direction d, float f);

    // DIVISIONES
    static Direction div(Direction d, float f);

    // MODULO
    static float mod(Direction d);

    // NORMALIZACIÓN
    static Direction norm(Direction d);

    // DOT (Producto punto)
    static float dot(Direction d1, Direction d2);
    static float dot(Direction d, Vector3 v); // <-- AÑADIDA: Para manejar el Vector3 devuelto por Ray

    // PRODUCTO CRUZ
    static Direction cross(Direction d1, Direction d2);

    //REFLEJAR
    static Direction reflect(Direction d1, Direction d2);
    
    // REFRACTAR
    static bool refract(const Vector3& I, const Vector3& N, float etaI, float etaT, Vector3& out);
    // Fresnel para dieléctricos (unpolarized)
    static float fresnelDielectric(float cosThetaI, float etaI, float etaT);
    
    // MOSTRAR POR PANTALLA
    static void show(Point p);
    static void show(Direction d);

    static Direction randomHemisphereDirection(const Direction& normal);
    static Vector3 sampleUniformSphereDirection();

};