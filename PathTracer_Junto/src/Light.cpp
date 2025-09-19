#define _USE_MATH_DEFINES
#include "Light.hpp"

//POr defecto
Light::Light(){}

//Luz puntual en un punto
Light::Light(Point origen, Pixel power) : origen(origen), power(power) {}

Light::Light(const Vector3& origenVec, const Pixel& powerPixel)
    : origen(Point(origenVec)), power(powerPixel) {}

// Luz en un area
// Luz en un área rectangular
Light::Light(Point centro, const Vector3& normal, float width, float height, const Pixel& powerPixel)
    : origen(centro), normal(normal), width(width), height(height), power(powerPixel) {}



Pixel Light::calcularIntensidadLuz(Point p) const {
    double denominador = pow(Operations::mod(Operations::sub(origen, p)), 2);
    float r = power.r / denominador;
    float g = power.g / denominador;
    float b = power.b / denominador;
    return Pixel{r, g, b};
}

Pixel Light::BRDF(Pixel color) const {
    return Pixel{
        color.r / float(M_PI),
        color.g / float(M_PI),
        color.b / float(M_PI)
    };
}

float Light::LambertianCos(Direction normal, Point p) const {
    Direction numerador = Operations::sub(origen, p);
    numerador = Operations::norm(numerador);
    float coseno = fabs(Operations::dot(numerador, normal));
    return coseno;
}

Pixel Light::calcularLuz(Point p, Direction normal, Pixel color) const {
    Pixel intensidad = calcularIntensidadLuz(p);
    float coseno = LambertianCos(normal, p);
    Pixel brdf = BRDF(color);
    return Pixel{
        (intensidad.r * brdf.r * coseno),
        (intensidad.g * brdf.g * coseno),
        (intensidad.b * brdf.b * coseno)
    };
}

// ----------- Métodos de área rectangular -----------

Point Light::samplePointOnArea(float xi1, float xi2) const {
    // Si no es área, devolvemos el origen
    if (width == 0 || height == 0) return origen;

    // Creamos un sistema ortonormal con la normal
    Vector3 up = fabs(normal.y) < 0.999f ? Vector3(0,1,0) : Vector3(1,0,0);
    Vector3 tangent = Operations::norm(Operations::cross(up, normal)).toVector();
    Vector3 bitangent = Operations::cross(normal, tangent).toVector();

    // Desplazamos con xi1, xi2 en [-0.5, 0.5]
    float dx = (xi1 - 0.5f) * width;
    float dy = (xi2 - 0.5f) * height;

    Vector3 sample = origen.toVector() + tangent * dx + bitangent * dy;
    return Point(sample);
}

Pixel Light::calcularIntensidadLuz(Point p, const Point& lightSample) const {
    double dist2 = pow(Operations::mod(Operations::sub(lightSample, p)), 2);
    float r = power.r / dist2;
    float g = power.g / dist2;
    float b = power.b / dist2;
    return Pixel{r, g, b};
}


Pixel Light::calcularLuzAreaSample(Point p, Direction normal, Pixel color, const Point& lightSample) const {
    Pixel intensidad = calcularIntensidadLuz(p, lightSample);

    // Dirección desde el punto hacia la luz
    Direction L = Operations::sub(lightSample, p);
    L = Operations::norm(L);

    float coseno = fabs(Operations::dot(L, normal));

    Pixel brdf = BRDF(color);

    return Pixel{
        intensidad.r * brdf.r * coseno,
        intensidad.g * brdf.g * coseno,
        intensidad.b * brdf.b * coseno
    };
}
