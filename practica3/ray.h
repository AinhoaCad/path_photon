#pragma once

#include <iostream>
#include "../practica1/point.h"
#include "../practica1/operations.h"

class Ray{
    public:
        Point origen;
        Direction direccion;
    Ray(Point origen, Direction direccion):origen(origen),direccion(direccion){}
    
};