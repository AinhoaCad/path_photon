#pragma once

#include "Image.hpp"
#include "Point.hpp"
#include "Vector3.hpp"
#include "Direction.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "AABB.hpp"

class Primitive {
public:
    virtual bool intersect(Ray& rayo, float &t) const = 0;
    virtual Material getMaterial() const = 0;
    virtual Direction getPointNormal(Point p) const = 0;

    virtual AABB boundingBox() const = 0;

    virtual bool emitsLight() const = 0;
    //virtual ~Primitive() {}
    
};

