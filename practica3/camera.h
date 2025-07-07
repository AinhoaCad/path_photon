#pragma once

#include "../practica1/point.h"
#include "../practica1/direction.h"
#include "../practica1/operations.h"
#include "../practica2/Image.h"
#include "ray.h"
#include "primitive.h"

class Camera {
public:
    Point origen;
    Direction left;
    Direction up;
    Direction forward;

    int pixelWidth;    // Número de píxeles a lo ancho
    int pixelHeight;   // Número de píxeles a lo alto
    const float anchoPlano = 2.0f;
    const float altoPlano = 2.0f;
    float onepixelWidth;
    float onepixelHeight;

    Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp);

    // Método para generar un rayo hacia el centro de un píxel dado (i, j)
    Ray generateRay(int i, int j, std::string n);

    Image generateImage(Primitive* primitives[], int numPrimitives);
    Image generateImageMultipleRays(Primitive* primitives[], int numPrimitives, int numRays);
};