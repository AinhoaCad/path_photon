#pragma once

#include <iostream>
#include "../practica1/point.h"
#include "../practica1/operations.h"
#include "../practica2/Image.h"
#include "ray.h"

class Primitive {
public:
    // Método virtual para la intersección
    virtual bool intersect(Ray rayo, float &t) const = 0;

    // Método virtual para la intersección
    virtual Pixel getColor() const = 0;

    virtual Direction getNormal(Point p) const = 0;

    virtual bool getEmision() const = 0;
    // Destructor virtual
    virtual ~Primitive() {}
};

class Sphere : public Primitive {
private:
    Point centro;
    float radio;
    Pixel color;
    bool emision;

public:
    // Constructor
    Sphere(Point c, float r, Pixel col, bool emision);

    // Getters
    Point getCentro() const;
    float getRadio() const;
    Pixel getColor() const override;
    Direction getNormal(Point p) const;
    bool getEmision() const override;

    // Setters
    void setCentro(Point c);
    void setRadio(float r);
    void setColor(Pixel col);
    
    // Sobrescribe el método de intersección
    bool intersect(Ray rayo, float &t) const override;
};

class Plane : public Primitive {
private:
    float distancia;
    Direction normal;
    Pixel color;
    bool emision;


public:
    // Constructor
    Plane(float d, Direction n, Pixel col, bool emision);

    // Getters
    float getDistancia() const;
    Direction getNormal(Point p) const override;
    Pixel getColor() const override;
    bool getEmision() const override;

    // Setters
    void setDistancia(float d);
    void setNormal(Direction n);
    void setColor(Pixel col);

    // Sobrescribe el método de intersección
    bool intersect(Ray rayo, float &t) const override;
};