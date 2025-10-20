#define _USE_MATH_DEFINES
#include "Light.hpp"
#include "Material.hpp"


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
    float coseno = (Operations::dot(numerador, normal));

    //Si es menor que 0 devolvemos 0
    if (coseno < 0) {
        return 0.0f;
    }   
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

Pixel Light::calcularLuzBSDF(Point p, Direction normal, const Material& material, const Direction& wi, const Direction& wo) const {
    // Si el material es emisor, devuelve su emisión directamente
    if (material.Le.R > 0 || material.Le.G > 0 || material.Le.B > 0) {
        return material.Le.toPixel();
    }

    // Intensidad de la luz (inversa cuadrada)
    Pixel intensidad = calcularIntensidadLuz(p);

    // Coseno entre normal y dirección de luz
    float coseno = LambertianCos(normal, p);

    // ---- Componente difusa (Lambertiana) ----
    Pixel brdf_difusa = {
        material.kd.R / float(M_PI),
        material.kd.G / float(M_PI),
        material.kd.B / float(M_PI)
    };

    // ---- Componente especular perfecta ----
    // Reflección perfecta ωr = reflect(-wi, n)
    Direction reflected = Operations::reflect(Operations::mult(wi, -1.0f), normal);
    float specFactor = std::max(0.0f, Operations::dot(reflected, wo));
    Pixel brdf_especular = {
        material.ks.R * specFactor,
        material.ks.G * specFactor,
        material.ks.B * specFactor
    };

    // ---- Componente transmisiva (refracción perfecta) ----
    Pixel brdf_transmisiva = {0, 0, 0};
    if (material.kt.R > 0 || material.kt.G > 0 || material.kt.B > 0) {
        Direction refracted;
        bool ok = Operations::refract(Operations::mult(wi, -1.0f), normal, material.ior, refracted);
        if (ok) {
            float transFactor = std::max(0.0f, Operations::dot(refracted, wo));
            brdf_transmisiva = {
                material.kt.R * transFactor,
                material.kt.G * transFactor,
                material.kt.B * transFactor
            };
        }
    }

    // ---- Sumar las tres contribuciones ponderadas ----
    Pixel total;
    total.r = intensidad.r * coseno * (brdf_difusa.r + brdf_especular.r + brdf_transmisiva.r);
    total.g = intensidad.g * coseno * (brdf_difusa.g + brdf_especular.g + brdf_transmisiva.g);
    total.b = intensidad.b * coseno * (brdf_difusa.b + brdf_especular.b + brdf_transmisiva.b);

    return total;
}


// ----------- Métodos de área rectangular -----------

Pixel Light::calcularRadianciaRectangular() const {
    // Si no es área, devolvemos la potencia directamente
    if (width == 0 || height == 0) return power;

    float area = width * height;
    return Pixel{
        power.r / (area * float(M_PI)),
        power.g / (area * float(M_PI)),
        power.b / (area * float(M_PI))
    };
}

Point Light::samplePointOnArea(float xi1, float xi2) const {
    // Si no es área, devolvemos el origen
    if (width == 0 || height == 0) return origen;

    // Creamos un sistema ortonormal con la normal
    Vector3 up = fabs(normal.y) < 0.999f ? Vector3(0,1,0) : Vector3(1,0,0);
    Vector3 tangent = Operations::norm(Operations::cross(normal, up)).toVector();
    Vector3 bitangent = Operations::norm(Operations::cross(normal, tangent)).toVector();

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
    // ===============================
    // 1️⃣ Radiancia emitida L_e
    // ===============================
    // Para luz rectangular Lambertiana:
    // L_e = power / (área * π)
    Pixel Le = calcularRadianciaRectangular(); // función que convierte power en radiancia

    // ===============================
    // 2️⃣ Vector desde p hacia la muestra de la luz
    // ===============================
    Direction L = Operations::sub(lightSample, p);  // vector desde p hacia lightSample
    float dist2 = Operations::dot(L, L);            // distancia al cuadrado |x-p|^2
    L = Operations::norm(L);                        // dirección normalizada

    // ===============================
    // 3️⃣ Coseno en la superficie receptora
    // ===============================
    // θ_surface = ángulo entre normal de la superficie y dirección de la luz
    float cosThetaSurface = Operations::dot(normal, L);
    if (cosThetaSurface <= 0.0f) return Pixel{0.0f, 0.0f, 0.0f}; // superficie orientada al revés

    // ===============================
    // 4️⃣ Coseno en la luz (orientación de la luz hacia el punto)
    // ===============================
    // θ_light = ángulo entre normal de la luz y dirección desde la luz hacia el punto
    Direction lightToPoint = Operations::mult(L, -1.0f);
    float cosThetaLight = Operations::dot(this->normal, lightToPoint);
    if (cosThetaLight <= 0.0f) return Pixel{0.0f, 0.0f, 0.0f}; // luz mirando hacia atrás

    // ===============================
    // 5️⃣ Evaluación del BRDF
    // ===============================
    // fr(p, wi, wo) para superficie Lambertiana = albedo / π
    Pixel brdf = BRDF(color);

    // ===============================
    // 6️⃣ Contribución final de la muestra
    // ===============================
    // L_o(p) ≈ L_e * fr * cosThetaSurface * cosThetaLight / |x-p|^2
    Pixel contrib;
    contrib.r = Le.r * brdf.r * cosThetaSurface * cosThetaLight / dist2;
    contrib.g = Le.g * brdf.g * cosThetaSurface * cosThetaLight / dist2;
    contrib.b = Le.b * brdf.b * cosThetaSurface * cosThetaLight / dist2;

    return contrib;
}
