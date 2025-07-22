#pragma once

#include "Image.hpp"
#include "Point.hpp"
#include "Vector3.hpp"
#include "Direction.hpp"
#include "Material.hpp"
#include "Ray.hpp"

class Primitive {
public:
    virtual bool intersect(Ray& rayo, float &t) const = 0;
    virtual Material getMaterial() const = 0;
    virtual Direction getPointNormal(Point p) const = 0;
    virtual bool getEmision() const = 0;
    //virtual ~Primitive() {}
    
};

