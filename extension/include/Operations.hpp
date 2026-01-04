#pragma once



#include <iostream>
#include <math.h>
#include "Point.hpp"
#include "Direction.hpp"
#include <cmath>

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
    static float dot(Direction d, Point p);

    // PRODUCTO CRUZ
    static Direction cross(Direction d1, Direction d2);

    //REFLEJAR
    static Direction reflect(Direction d1, Direction d2);
    
    // REFRACTAR
    static bool refract(const Vector3& I, const Vector3& N, float etaI, float etaT, Vector3& out);
    // Fresnel para dieléctricos (unpolarized)
    // cosThetaI: coseno del ángulo entre la dirección incidente y la normal (valor absoluto)
    // etaI, etaT: índices de refracción incident/trasmitente
    static float fresnelDielectric(float cosThetaI, float etaI, float etaT);
    // MOSTRAR POR PANTALLA
    static void show(Point p);
    static void show(Direction d);

    static Direction randomHemisphereDirection(const Direction& normal);
    static Vector3 sampleUniformSphereDirection();

};