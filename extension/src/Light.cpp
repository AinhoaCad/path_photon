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

// ---------------- Cálculo de la contribución de la luz puntual
// Esta función corresponde a calcular el término:
// L_o(x, ω_o) ≈ (I_l / ||c_l - x||^2) · f_r(x, ω_i, ω_o) · max(0, n · ω_i)
//
// Donde:
// - L_o(x, ω_o) → Radiancia que llega al punto x desde la luz en dirección ω_i
// - I_l         → Intensidad de la luz puntual
// - f_r(x, ω_i, ω_o) → BRDF del material en x
// - max(0, n · ω_i)  → Coseno del ángulo de incidencia (Lambert)
Pixel Light::calcularLuzBSDF(
    Point p, 
    Direction normal, 
    const Material& material, 
    const Direction& wi,   // Dirección desde el punto hacia la luz (ω_i)
    const Direction& wo    // Dirección hacia la cámara (ω_o)
) const {
    // ---------------- Paso 0: materiales emisores ----------------
    // Si el material mismo emite luz, devolvemos su radiancia Le(x, ω_o)
    if (material.Le.R > 0 || material.Le.G > 0 || material.Le.B > 0) {
        return material.Le.toPixel();
    }

    // ---------------- Paso 1: intensidad de la luz I_l ----------------
    // Este término incluye la atenuación por distancia (inversa cuadrada)
    Pixel intensidad = calcularIntensidadLuz(p);

    // ---------------- Paso 2: coseno de incidencia -------------------
    // max(0, n · ω_i) → Lambert
    float coseno = LambertianCos(normal, p);

    // ---------------- Paso 3: BRDF difusa (Lambertiana) --------------
    // f_r(x, ω_i, ω_o) = k_d / π
    Pixel brdf_difusa = {
        material.kd.R / float(M_PI),
        material.kd.G / float(M_PI),
        material.kd.B / float(M_PI)
    };

    // ---------------- Paso 4: BRDF especular perfecta -----------------
    // Reflexión especular delta: f_r = k_s * max(0, dot(ω_r, ω_o))
    Direction reflected = Operations::reflect(Operations::mult(wi, -1.0f), normal);
    float specFactor = std::max(0.0f, Operations::dot(reflected, wo));
    Pixel brdf_especular = {
        material.ks.R * specFactor,
        material.ks.G * specFactor,
        material.ks.B * specFactor
    };

    // ---------------- Paso 5: BRDF transmisiva (refracción) -----------
    Pixel brdf_transmisiva = {0, 0, 0};
    if (material.kt.R > 0 || material.kt.G > 0 || material.kt.B > 0) {
        Direction incident = Operations::mult(wi, -1.0f);
        float cosTheta = Operations::dot(incident, normal);

        // Fresnel para ajustar reflexión / transmisión
        float etaI = 1.0f, etaT = material.ior;
        float absCosTh = std::fabs(cosTheta);

        if (cosTheta > 0.0f) {
            // saliendo del material
            etaI = material.ior; etaT = 1.0f;
        } else {
            // entrando al material
            absCosTh = -cosTheta;
            etaI = 1.0f; etaT = material.ior;
        }

        float R = Operations::fresnelDielectric(absCosTh, etaI, etaT);
        float T = 1.0f - R;

        Vector3 refracted;
        bool ok = Operations::refract(incident.toVector(), normal.toVector(), 1.0f, material.ior, refracted);

        if (ok && T > 0.0f) {
            float absCos = std::fabs(Operations::dot(refracted, normal));
            if (absCos < 1e-6f) absCos = 1e-6f;

            // Corrección de radiancia por cambio de medio: (η_i / η_t)^2
            float etaRatio = (cosTheta > 0.0f) ? (material.ior / 1.0f) : (1.0f / material.ior);
            float factor = (T / absCos) / (etaRatio * etaRatio);

            brdf_transmisiva = {
                material.kt.R * factor,
                material.kt.G * factor,
                material.kt.B * factor
            };
        }
    }

    // ---------------- Paso 6: combinación final ----------------------
    // Multiplicamos por intensidad de la luz y coseno de incidencia
    // L_o(x, ω_o) ≈ I_l · f_r(x, ω_i, ω_o) · max(0, n · ω_i)
    Pixel total;
    total.r = intensidad.r * coseno * (brdf_difusa.r + brdf_especular.r + brdf_transmisiva.r);
    total.g = intensidad.g * coseno * (brdf_difusa.g + brdf_especular.g + brdf_transmisiva.g);
    total.b = intensidad.b * coseno * (brdf_difusa.b + brdf_especular.b + brdf_transmisiva.b);

    return total;
}


// ----------- Métodos de área rectangular -----------

Pixel Light::calcularRadianciaRectangular() const {
    
    //Devolvemos power directamente 
    
    return power;

    // Si no es área, devolvemos la potencia directamente
    //if (width == 0 || height == 0) return power;

    /*float area = width * height;
    return Pixel{
        power.r / (area * float(M_PI)),
        power.g / (area * float(M_PI)),
        power.b / (area * float(M_PI))
    };*/
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
