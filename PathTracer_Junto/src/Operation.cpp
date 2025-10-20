#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>  // Para usar sqrt
#include "Operations.hpp"

// Implementación de las funciones
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

bool Operations::refract(const Direction& incident, const Direction& normal, float eta, Direction& out) {
    // Convert Direction -> Vector3 for using Vector3::refract
    Vector3 iVec = incident.toVector();
    Vector3 nVec = normal.toVector();
    Vector3 r = iVec.refract(nVec, eta);
    // Vector3::refract returns (0,0,0) when TIR occurs in that implementation
    if (r.x == 0 && r.y == 0 && r.z == 0) {
        return false;
    }
    out = r.toDirection();
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