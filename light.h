#pragma once

#include <iostream>
#include "../practica1/point.h"
#include "../practica2/Image.h"
#include "../practica1/operations.h"
#include <cmath>

class Light{
    public:
        Point origen;
        Pixel power;
    Light(Point origen, Pixel power):origen(origen),power(power){}

    Pixel calcularIntensidadLuz(Point p){
        double denominador = pow(Operations::mod(Operations::sub(origen,p)),2);
        float r = power.r/denominador;
        float g = power.g/denominador;
        float b = power.b/denominador;
        return Pixel{r,g,b};
    }
    Pixel BRDF(Pixel color){
        return Pixel{color.r / float(M_PI), color.g / float(M_PI), color.b / float(M_PI)};
    }
    
    float LambertianCos(Direction normal, Point p) {
        //float denominador = Operations::mod(Operations::sub(origen,p));
        Direction numerador = Operations::sub(origen,p);
        numerador = Operations::norm(numerador);
        float coseno = fabs(Operations::dot(numerador, normal));
        return coseno;
    }
    Pixel calcularLuz(Point p, Direction normal, Pixel color){
        Pixel intensidad = calcularIntensidadLuz(p);
        float coseno = LambertianCos(normal, p);
        Pixel brdf = BRDF(color);
        return Pixel{
            (intensidad.r * brdf.r * coseno),
            (intensidad.g * brdf.g * coseno),
            (intensidad.b * brdf.b * coseno)
        };
    }
};
