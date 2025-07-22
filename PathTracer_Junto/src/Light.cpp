#define _USE_MATH_DEFINES
#include "Light.hpp"


Light::Light(Point origen, Pixel power) : origen(origen), power(power) {}

Light::Light(const Vector3& origenVec, const Pixel& powerPixel)
    : origen(Point(origenVec)), power(powerPixel) {}

Pixel Light::calcularIntensidadLuz(Point p) {
    double denominador = pow(Operations::mod(Operations::sub(origen, p)), 2);
    float r = power.r / denominador;
    float g = power.g / denominador;
    float b = power.b / denominador;
    return Pixel{r, g, b};
}

Pixel Light::BRDF(Pixel color) {
    return Pixel{
        color.r / float(M_PI),
        color.g / float(M_PI),
        color.b / float(M_PI)
    };
}

float Light::LambertianCos(Direction normal, Point p) {
    Direction numerador = Operations::sub(origen, p);
    numerador = Operations::norm(numerador);
    float coseno = fabs(Operations::dot(numerador, normal));
    return coseno;
}

Pixel Light::calcularLuz(Point p, Direction normal, Pixel color) {
    Pixel intensidad = calcularIntensidadLuz(p);
    float coseno = LambertianCos(normal, p);
    Pixel brdf = BRDF(color);
    return Pixel{
        (intensidad.r * brdf.r * coseno),
        (intensidad.g * brdf.g * coseno),
        (intensidad.b * brdf.b * coseno)
    };
}