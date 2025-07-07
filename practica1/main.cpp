#include <iostream>
#include "operations.cpp"  

int main() {
    // Crear puntos y direcciones de ejemplo
    Point p1(1, 2, 3);
    Point p2(4, 5, 6);
    Direction d1(1, 0, 0);
    Direction d2(0, 1, 0);

    // Operaciones básicas
    Point p3 = Operations::add(p1, d1); // Sumar punto y dirección
    Operations::show(p3);  // Mostrar el resultado de sumar un punto y una dirección

    Direction d3 = Operations::add(d1, d2); // Sumar dos direcciones
    Operations::show(d3);  // Mostrar el resultado de sumar dos direcciones

    Direction d4 = Operations::sub(p2, p1); // Restar dos puntos
    Operations::show(d4);  // Mostrar el resultado de restar dos puntos

    // Multiplicación y división de dirección por un escalar
    Direction d5 = Operations::mult(d1, 5.0); // Multiplicar una dirección por un escalar
    Operations::show(d5);  // Mostrar el resultado

    Direction d6 = Operations::div(d5, 5.0); // Dividir una dirección por un escalar
    Operations::show(d6);  // Mostrar el resultado

    // Módulo (magnitud) de una dirección
    float mod_d1 = Operations::mod(d1); // Calcular el módulo de la dirección d1
    std::cout << "Modulo de d1: " << mod_d1 << std::endl;

    // Normalización de una dirección
    Direction d7 = Operations::norm(d1); // Normalizar la dirección d1
    Operations::show(d7);  // Mostrar el resultado de la normalización

    // Producto punto (dot product)
    float dotProduct = Operations::dot(d1, d2); // Calcular producto punto entre d1 y d2
    std::cout << "Producto punto de d1 y d2: " << dotProduct << std::endl;

    // Producto cruz (cross product)
    Direction d8 = Operations::cross(d1, d2); // Calcular producto cruz entre d1 y d2
    Operations::show(d8);  // Mostrar el resultado del producto cruz

    return 0;
}
