#include "cylinder.hpp"

Cylinder::Cylinder(Direction d, Point c, float h, float r, Material _material) : direction(d),center(c),heigth(h),radius(r),Primitive(_material)
{
}

#include "cylinder.hpp"
#include <cmath>
#include <limits>


bool Cylinder::doesIntersect(Ray r, Intersection &intersection) {
    Point rayOrigin = r.getPoint();        
    Direction rayDir = r.getDirection();   
    Direction cylinderDir = direction;     
    Direction delta = rayOrigin - center;
    Direction rayDirProj = rayDir - (cylinderDir*rayDir.dot_product(cylinderDir));
    Direction deltaProj = delta - (cylinderDir*delta.dot_product(cylinderDir));
    RGB k_d= getK_d();
    RGB k_s=getK_s();
    RGB k_t=getK_t();
    RGB k_e=getK_e();
    float n=getRefractionIndex();
    // Coeficientes de la ecuación cuadrática
    float a = rayDirProj.dot_product(rayDirProj);
    float b = 2.0f * rayDirProj.dot_product(deltaProj);
    float c = deltaProj.dot_product(deltaProj) - (radius * radius);

    // Resolver la ecuación cuadrática
    float discriminant = b * b - 4 * a * c;

    // Si el discriminante es negativo, no hay intersección
    if (discriminant < 0) {
        return false;
    }

    // Calcular las posibles soluciones de t (distancias a lo largo del rayo)
    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2 * a);
    float t2 = (-b + sqrtDisc) / (2 * a);

    // Asegurarse de que al menos uno de los t sea positivo (el rayo va hacia el cilindro)
    if (t1 < 0 && t2 < 0) {
        return false;
    }

    // Tomar el valor más cercano de t que sea positivo
    float t = (t1 > 0) ? t1 : t2;

    // Calcular el punto de intersección
    Point intersectionPoint = rayOrigin +  rayDir*t;

    // Proyectar el punto en la dirección del cilindro para verificar que está dentro de los límites de altura
    float projectionHeight = (intersectionPoint - center).dot_product(cylinderDir);
    if (projectionHeight < 0 || projectionHeight > heigth) {
        return false; // Está fuera de los límites del cilindro
    }

    intersection=Intersection(t, intersectionPoint, intersectionPoint - (center +   cylinderDir*projectionHeight), Material(k_d,k_s,k_t,k_e,n));

    return true;
}
