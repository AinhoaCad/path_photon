#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>  // Para usar sqrt
#include "Operations.hpp"


Point Operations::add(Point p, Direction d) {
    return Point(p.getX() + d.getX(), p.getY() + d.getY(), p.getZ() + d.getZ());
}

Direction Operations::add(Direction d1, Direction d2) {
    return Direction(d1.getX() + d2.getX(), d1.getY() + d2.getY(), d1.getZ() + d2.getZ());
}

Direction Operations::sub(Point p1, Point p2) {
    return Direction(p1.getX() - p2.getX(), p1.getY() - p2.getY(), p1.getZ() - p2.getZ());
}

Direction Operations::sub(Direction d1, Direction d2) {
    return Direction(d1.getX() - d2.getX(), d1.getY() - d2.getY(), d1.getZ() - d2.getZ());
}

Point Operations::sub(Point p, Direction d) {
    return Point(p.getX() - d.getX(), p.getY() - d.getY(), p.getZ() - d.getZ());
}

Direction Operations::mult(Direction d, float f) {
    return Direction(d.getX() * f, d.getY() * f, d.getZ() * f);
}

Direction Operations::div(Direction d, float f) {
    return Direction(d.getX() / f, d.getY() / f, d.getZ() / f);
}

float Operations::mod(Direction d) {
    return sqrt(d.getX() * d.getX() + d.getY() * d.getY() + d.getZ() * d.getZ());
}

Direction Operations::norm(Direction d) {
    float magnitude = mod(d);
    return Direction(d.getX() / magnitude, d.getY() / magnitude, d.getZ() / magnitude);
}

float Operations::dot(Direction d1, Direction d2) {
    return (d1.getX() * d2.getX() + d1.getY() * d2.getY() + d1.getZ() * d2.getZ());
}

float Operations::dot(Direction d, Point p) {
    return (d.getX() * p.getX() + d.getY() * p.getY() + d.getZ() * p.getZ());
}

Direction Operations::cross(Direction d1, Direction d2) {
    return Direction(d1.getY() * d2.getZ() - d1.getZ() * d2.getY(),
                     d1.getZ() * d2.getX() - d1.getX() * d2.getZ(),
                     d1.getX() * d2.getY() - d1.getY() * d2.getX());
}

Direction Operations::reflect(Direction incident, Direction normal) {
    float dotProduct = Operations::dot(incident, normal);
    return Operations::sub(incident, Operations::mult(normal, 2 * dotProduct));
}

bool Operations::refract(const Vector3& I, const Vector3& N, float etaI, float etaT, Vector3& out) {
    // I: dirección incidente normalizada (apuntando hacia la superficie)
    // N: normal de la superficie
    // etaI: índice del medio desde donde viene el rayo
    // etaT: índice del medio hacia donde va el rayo
    
    Vector3 incident = I.normalized();
    Vector3 normal = N.normalized();
    
    // Calcular el producto punto entre incidente y normal
    float cosI = incident.dotProduct(normal);
    
    // Ajustar normal y ratio de índices según si entramos o salimos
    float etaRatio;
    if (cosI < 0.0f) {
        // Entrando al material: el rayo y la normal apuntan en direcciones opuestas
        cosI = -cosI;
        etaRatio = etaI / etaT;
    } else {
        // Saliendo del material: el rayo y la normal apuntan en la misma dirección
        normal = normal * -1.0f;
        etaRatio = etaT / etaI;
    }
    
    // Verificar reflexión interna total usando ley de Snell
    float k = 1.0f - etaRatio * etaRatio * (1.0f - cosI * cosI);
    if (k < 0.0f) {
        return false; // TIR (Total Internal Reflection)
    }
    
    // Calcular dirección refractada
    out = (incident * etaRatio + normal * (etaRatio * cosI - std::sqrt(k))).normalized();
    return true;
}


void Operations::show(Point p) {
    std::cout << p << std::endl;
}

void Operations::show(Direction d) {
    std::cout << d << std::endl;
}

Direction Operations::randomHemisphereDirection(const Direction& normal) {
    float u = static_cast<float>(rand()) / RAND_MAX;
    float v = static_cast<float>(rand()) / RAND_MAX;

    float theta = acos(sqrt(1.0f - u));
    float phi = 2.0f * M_PI * v;

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    // Coordenadas locales
    Direction local(x, y, z);

    // Construir base ortonormal
    Direction up = normal;
    Direction right = Operations::norm(Operations::cross(up, Direction(0.0072, 1.0, 0.0034)));
    Direction forward = Operations::cross(right, up);

    Direction world = Operations::add(
        Operations::add(
            Operations::mult(right, x),
            Operations::mult(forward, y)
        ),
        Operations::mult(up, z)
    );

    return Operations::norm(world);
}

/**
 * Calcula el coeficiente de Fresnel para un dieléctrico no polarizado.
 * 
 * Esta función determina la fracción de luz que se refleja en la interfaz 
 * entre dos materiales dieléctricos (por ejemplo, aire y vidrio), usando 
 * la ley de Fresnel. Considera reflexión interna total y el ángulo de 
 * incidencia entrante o saliente.
 * 
 * @param cosThetaI Coseno del ángulo de incidencia respecto a la normal de la superficie.
 *                  Debe estar en el rango [-1, 1]. Si es negativo, indica que el rayo
 *                  entra desde dentro del material.
 * @param etaI Índice de refracción del medio de origen (de donde viene el rayo).
 * @param etaT Índice de refracción del medio de destino (hacia donde va el rayo).
 * 
 * @return float Coeficiente de reflexión Fresnel (R), en el rango [0,1].
 *               Representa la fracción de luz reflejada; la fracción transmitida 
 *               es 1 - R.
 * 
 * @note La función maneja:
 *       - Reflexión total interna (R = 1)
 *       - Intercambio de índices si el rayo sale del material
 *       - Promedio de componentes paralela y perpendicular para luz no polarizada
 * 
 * @warning Se espera que los índices de refracción sean positivos y no nulos.
 * 
 */
float Operations::fresnelDielectric(float cosThetaI, float etaI, float etaT) {
    // Clamp cosThetaI al rango [-1, 1]
    // Clamp manual de cosThetaI al rango [-1, 1]
    if (cosThetaI < -1.0f) cosThetaI = -1.0f;
    else if (cosThetaI > 1.0f) cosThetaI = 1.0f;

    bool entering = cosThetaI > 0.0f;
    // Si estamos saliendo del material, intercambiamos los índices
    if (!entering) {
        std::swap(etaI, etaT);
        cosThetaI = std::fabs(cosThetaI);
    }

    // Ley de Snell
    float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;

    // Reflexión total interna
    if (sinThetaT >= 1.0f) {
        return 1.0f;
    }

    float cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

    // Coeficientes de Fresnel (no polarizados)
    float rParallel = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                      ((etaT * cosThetaI) + (etaI * cosThetaT));
    float rPerp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                  ((etaI * cosThetaI) + (etaT * cosThetaT));

    return 0.5f * (rParallel * rParallel + rPerp * rPerp);
}

Vector3 Operations::sampleUniformSphereDirection() {
        // Generación de números aleatorios [0, 1]
        float r1 = (float)rand() / (float)RAND_MAX;
        float r2 = (float)rand() / (float)RAND_MAX;
        
        // Transformación esférica (Método de Muestreo de la Inversa de la CDF)
        float theta = acosf(1.0f - 2.0f * r1); // acos(1 - 2*r1)
        float phi = 2.0f * M_PI * r2;

        // Conversión a coordenadas cartesianas
        Vector3 dir(
            sinf(theta) * cosf(phi),
            sinf(theta) * sinf(phi),
            cosf(theta)
        );
        return dir;
    }

