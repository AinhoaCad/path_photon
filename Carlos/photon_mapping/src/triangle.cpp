#include "triangle.hpp"

Triangle::Triangle(Point _v1, Point _v2, Point _v3, Material _material)
    : v1(_v1), v2(_v2), v3(_v3), Primitive(_material)
{
    Direction edge1 = v2 - v1;
    Direction edge2 = v3 - v1;
    normal = edge1.cross_product(edge2).normalization();

}

bool Triangle::doesIntersect(Ray r, Intersection &intersection) {
    RGB k_d= getK_d();
    RGB k_s=getK_s();
    RGB k_t=getK_t();
    RGB k_e=getK_e();
    float n=getRefractionIndex();
    float denom = normal.dot_product(r.getDirection());

    // Check if ray and triangle plane are parallel
    if (std::abs(denom) < 1e-6) return false;

    // Calculate d in plane equation (ax + by + cz + d = 0)
    float d = -normal.dot_product(v1-Point(0,0,0));

    // Calculate the intersection parameter t
    float t = -(normal.dot_product(r.getPoint()-Point(0,0,0)) + d) / denom;

    // Ensure the intersection point is in front of the ray origin
    if (t < 1e-6) return false;

    // Calculate the intersection point
    Point interPoint = r.getPoint() + r.getDirection() * t;

    // Inside-Outside Test using cross products
    Direction edge1 = v2 - v1;
    Direction vp1 = interPoint - v1;
    if (normal.dot_product(edge1.cross_product(vp1)) < 0) return false;

    Direction edge2 = v3 - v2;
    Direction vp2 = interPoint - v2;
    if (normal.dot_product(edge2.cross_product(vp2)) < 0) return false;

    Direction edge3 = v1 - v3;
    Direction vp3 = interPoint - v3;
    if (normal.dot_product(edge3.cross_product(vp3)) < 0) return false;

    // If inside all edges, intersection is valid
    intersection = Intersection(t, interPoint, normal, Material(k_d,k_s,k_t,k_e,n));
    return true;
}
