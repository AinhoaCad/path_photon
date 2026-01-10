#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include "Operations.hpp"
#include "Vector3.hpp"

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
    return d1.getX() * d2.getX() + d1.getY() * d2.getY() + d1.getZ() * d2.getZ();
}

float Operations::dot(Direction d, Vector3 v) {
    return d.getX() * v.x + d.getY() * v.y + d.getZ() * v.z;
}

Direction Operations::cross(Direction d1, Direction d2) {
    return Direction(
        d1.getY() * d2.getZ() - d1.getZ() * d2.getY(),
        d1.getZ() * d2.getX() - d1.getX() * d2.getZ(),
        d1.getX() * d2.getY() - d1.getY() * d2.getX()
    );
}

Direction Operations::reflect(Direction incident, Direction normal) {
    float dotProduct = Operations::dot(incident, normal);
    return Operations::sub(incident, Operations::mult(normal, 2 * dotProduct));
}

bool Operations::refract(const Vector3& I, const Vector3& N, float etaI, float etaT, Vector3& out) {
    // Implementación estándar con manejo de entrada/salida y TIR
    Vector3 incident = I.normalized();
    Vector3 normal = N.normalized();

    float cosI = incident.dotProduct(normal);
    float etaRatio;

    if (cosI < 0.0f) {
        // Entrando al material
        cosI = -cosI;
        etaRatio = etaI / etaT;
    } else {
        // Saliendo del material: invertimos la normal
        normal = normal * -1.0f;
        etaRatio = etaT / etaI;
    }

    float k = 1.0f - etaRatio * etaRatio * (1.0f - cosI * cosI);
    if (k < 0.0f) return false; // Reflexión interna total

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

    Direction local(x, y, z);

    Direction up = normal;
    Direction right = Operations::norm(Operations::cross(up, Direction(0.0072f, 1.0f, 0.0034f)));
    Direction forward = Operations::cross(right, up);

    Direction world = Operations::add(
        Operations::add(Operations::mult(right, x), Operations::mult(forward, y)),
        Operations::mult(up, z)
    );

    return Operations::norm(world);
}

/**
 * Calcula el coeficiente de Fresnel R para un dieléctrico no polarizado,
 * a partir del coseno del ángulo incidente y los índices de refracción
 * (η_i, η_t), usando la ley de Snell y el promedio de las componentes
 * paralela y perpendicular de Fresnel, incluyendo reflexión total interna.
 */
float Operations::fresnelDielectric(float cosThetaI, float etaI, float etaT) {
    if (cosThetaI < -1.0f) cosThetaI = -1.0f;
    else if (cosThetaI > 1.0f) cosThetaI = 1.0f;

    bool entering = cosThetaI > 0.0f;
    if (!entering) {
        std::swap(etaI, etaT);
        cosThetaI = std::fabs(cosThetaI);
    }

    float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;

    if (sinThetaT >= 1.0f) return 1.0f;

    float cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

    float rParallel = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
    float rPerp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

    return 0.5f * (rParallel * rParallel + rPerp * rPerp);
}

Vector3 Operations::sampleUniformSphereDirection() {
    float r1 = static_cast<float>(rand()) / RAND_MAX;
    float r2 = static_cast<float>(rand()) / RAND_MAX;

    float theta = acosf(1.0f - 2.0f * r1);
    float phi = 2.0f * M_PI * r2;

    Vector3 dir(
        sinf(theta) * cosf(phi),
        sinf(theta) * sinf(phi),
        cosf(theta)
    );

    return dir;
}
