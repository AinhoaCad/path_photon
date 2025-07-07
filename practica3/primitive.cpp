#include <iostream>
#include "primitive.h"
#include "ray.h"
#include "../practica2/Image.h"
#include <cmath>

// Constructor de Sphere
Sphere::Sphere(Point c, float r, Pixel col, bool e) : centro(c), radio(r), color(col), emision(e) {}

// Getters de Sphere
Point Sphere::getCentro() const {
    return centro;
}

float Sphere::getRadio() const {
    return radio;
}

Pixel Sphere::getColor() const {
    return color;
}

Direction Sphere::getNormal(Point p) const {
    Operations operador;
    return operador.norm(operador.sub(p, centro));
}

bool Sphere::getEmision()const {
    return emision;
}

// Setters de Sphere
void Sphere::setCentro(Point c) {
    centro = c;
}

void Sphere::setRadio(float r) {
    radio = r;
}

void Sphere::setColor(Pixel col) {
    color = col;
}

// Método para calcular la intersección rayo-esfera
bool Sphere::intersect(Ray rayo, float &t1) const {
    Operations operador;
    Direction oc = operador.sub(rayo.origen, centro);
    float a = operador.dot(rayo.direccion, rayo.direccion);
    float b = 2.0f * operador.dot(oc, rayo.direccion);
    float c = operador.dot(oc, oc) - radio * radio;
    float discriminant = b * b - 4 * a * c;
    

    if (discriminant < 0) {
                //printf("rayo.origen = (%f, %f, %f)\n", rayo.origen.getX(), rayo.origen.getY(), rayo.origen.getZ());
        //printf("rayo.direccion = (%f, %f, %f)\n", rayo.direccion.getX(), rayo.direccion.getY(), rayo.direccion.getZ());
        return false;
    } else {
        float t1_temp = (-b - sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + sqrt(discriminant)) / (2.0f * a);

        // Caso 1: ambos t son positivos. Tomamos el menor de ellos.
        if (t1_temp > 0 && t2 > 0) {
             t1 = (t1_temp < t2) ? t1_temp : t2;
        }
        // Caso 2: uno de los t es negativo. Tomamos el positivo.
        else if (t1_temp > 0) {
            t1 = t1_temp;
        }
        else if (t2 > 0) {
            t1 = t2;
        }
        // Caso 3: ambos t son negativos. No hay intersección.
        else {
            return false;
        }

        return true;
    }
}

// Constructor de Plane
Plane::Plane(float d, Direction n, Pixel col, bool e) : distancia(d), normal(n), color(col), emision(e) {}

// Getters de Plane
float Plane::getDistancia() const {
    return distancia;
}

Direction Plane::getNormal(Point p) const {
    return normal;
}

Pixel Plane::getColor() const {
    return color;
}

bool Plane::getEmision()const {
    return emision;
}

// Setters de Plane
void Plane::setDistancia(float d) {
    distancia = d;
}

void Plane::setNormal(Direction n) {
    normal = n;
}

void Plane::setColor(Pixel col) {
    color = col;
}

// Método para calcular la intersección rayo-plano
bool Plane::intersect(Ray rayo, float &t1) const {
    Operations operador;
    float denominador = operador.dot(normal, rayo.direccion);

    if (denominador == 0) {
        return false;
    }
    float numerador = -(distancia + operador.dot(normal, rayo.origen));
    t1 = numerador / denominador;
    if (t1 < 0) {
        return false;   // La intersección está detrás del origen del rayo
    }

    return true;
}