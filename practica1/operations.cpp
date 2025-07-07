#include <iostream>
#include <cmath>  // Para usar sqrt
#include "operations.h"

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
void Operations::show(Point p) {
    std::cout << p << std::endl;
}

void Operations::show(Direction d) {
    std::cout << d << std::endl;
}