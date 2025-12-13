#include "plane.hpp"
#include "ray.hpp"

Plane::Plane(Direction n, float d, Material _material): normal(n), distancetoOrigin(d), Primitive( _material){

}


bool Plane::doesIntersect(Ray r, Intersection &intersection)
{
    float denom = (normal).dot_product(r.getDirection());
    RGB k_d= getK_d();
    RGB k_s=getK_s();
    RGB k_t=getK_t();
    RGB k_e=getK_e();
    float n=getRefractionIndex();

    // Verificamos que el rayo no sea paralelo al plano
    if (std::abs(denom) > 10e-5) {
        // Creamos un vector desde el origen del rayo hacia el plano
        //Direction rayToPlane =  r.getPoint()- Point(0, 0, 0);
        Direction rayToPlane(r.getPoint()[0],r.getPoint()[1],r.getPoint()[2]);
        // Calculamos el parámetro t correctamente
        float t = (-rayToPlane.dot_product(normal) - distancetoOrigin) / denom;
        // Comprobamos que la intersección está delante del origen del rayo
        if (t > 10e-5) {
            // Calculamos el punto de intersección
            Direction _normal=Direction(normal[0],normal[1],normal[2]);
            Point interPoint = r.getPoint() + r.getDirection() * t;
            /*if (denom<0){
                _normal= _normal*(-1.0);
            }*/
            // Creamos la intersección
            intersection = Intersection(t, interPoint, _normal.normalization(), Material(k_d,k_s,k_t,k_e,n));
            return true;
        }
        
    }
    return false;
}

