#include <iostream>
#include "../practica1/point.h"
#include "primitive.h"
#include "ray.h"
#include "camera.h"


int main() {
    //Definir la camara
    Point origen(0, 0, -3.5);
    Direction l(-1, 0, 0);
    Direction u(0, 1, 0);
    Direction f(0, 0, 3);
    Camera camara(origen, l, u, f, 256, 256);
    
    //Definir el plano
    float d = 1.0f;

    Direction nl(1, 0, 0);
    Pixel coll = {1, 0, 0}; //rojo
    Plane planol(d,nl,coll,false); 
  
    Direction nr(-1, 0, 0);
    Pixel colr = {0, 1, 0};  //verde
    Plane planor(d,nr,colr,false);

    Direction nf(0, 1, 0);
    Pixel colf = {0.5, 0.5, 0.5}; //gris
    Plane planof(d,nf,colf,false);

    Direction nc(0, -1, 0);
    Pixel colc = {0.5, 0.5, 0.5}; //gris
    Plane planoc(d,nc,colc,false);

    Direction nb(0, 0, -1);
    Pixel colb = {0.5, 0.5, 0.5}; //gris
    Plane planob(d,nb,colb,false);
    
    
    //Definir las esfera
    Point lcentro(-0.5, -0.7, 0.25);
    float radio = 0.3;
    Pixel lcolesfera = {1, 0, 1}; //morado
    Sphere lesfera(lcentro, radio,lcolesfera,false);


    Pixel rcolesfera = {0, 0, 1}; //azul
    Point rcentro(0.5, -0.7, -0.25);
    Sphere resfera(rcentro, radio,rcolesfera,false);

    
    Primitive* primitivas[7];
    primitivas[0] = &planol;
    primitivas[1] = &resfera;
    primitivas[2] = &lesfera;
    primitivas[3] = &planor;
    primitivas[4] = &planof;
    primitivas[5] = &planoc;
    primitivas[6] = &planob;

    int n_primitivas = 7;

    Image img = camara.generateImage(primitivas, n_primitivas);
    img.writePPM("imagen.ppm");
    Image img64 = camara.generateImageMultipleRays(primitivas, n_primitivas, 64);
    img64.writePPM("imagen64.ppm");
    std::cout << "Imagen generada" << std::endl;
    return 0;
}