#include <iostream>
#include "../practica1/point.h"
#include "../practica1/operations.h"
using namespace std;

class Ray{
    public:
        Point origen;
        Direction direccion;
    Ray(Point origen, Direction direccion):origen(origen),direccion(direccion){}
    
};