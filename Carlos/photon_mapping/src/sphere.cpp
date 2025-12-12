#include "sphere.hpp"

Sphere::Sphere(Point c, Direction a, Material _material) : center(c), axis(a), Primitive(_material) {

}

Point Sphere::getCenter(){
    return center;
}

Direction Sphere::getAxis(){
    return axis;
}


bool Sphere::doesIntersect( Ray r, Intersection& intersection) 
{
    Point center = getCenter();
    float radius = getAxis().modulus();  // Asumimos que el radio está correctamente definido

    Direction oc = r.getPoint() - center;  // Vector desde el centro de la esfera al origen del rayo
    Direction dir = r.getDirection();      // Dirección del rayo
    
    float a = dir.dot_product(dir);        // Coeficiente a del cuadrado de t
    float b = 2.0f * oc.dot_product(dir);  // Coeficiente lineal b
    float c = oc.dot_product(oc) - radius * radius;  // Constante c

    // Material value
    RGB k_d= getK_d();
    RGB k_s=getK_s();
    RGB k_t=getK_t();
    RGB k_e=getK_e();
    float n=getRefractionIndex();


    // Discriminante de la ecuación cuadrática
    float discriminant = b * b - 4 * a * c;

    // Si el discriminante es negativo, no hay intersección
    if (discriminant < 0) {
        return false;
    }
    float t1;
    float t2;
    if (discriminant==0){
        t1=t2= -0.5f* b / a;
    }else{
        t1=0.5 * (-b + sqrt(discriminant))/a;
        t2=0.5 * (-b - sqrt(discriminant))/a;
    }
    if (t1 > t2) std::swap(t1, t2);
    if (t1 < 0) {
        t1 = t2; 
        if (t2 < 0) return false; 
    }
    float t = t1;
    // Calculamos el punto de intersección y la normal
    Point intersectionPoint = r.getPoint() + dir * t;
    Direction normal = (intersectionPoint - center).normalization();  // Normal en el punto de intersección
    intersection = Intersection(t, intersectionPoint, normal.normalization(), Material(k_d,k_s,k_t,k_e,n));
    return true;
}

