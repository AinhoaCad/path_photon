#include <iostream>
#include "../practica1/point.h"
#include "../practica3/primitive.h"
#include "../practica3/ray.h"
#include "../practica3/camera.h"
#include "light.h"


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
    Pixel lcolesfera = {0.5, 0, 0.5}; //morado
    Sphere lesfera(lcentro, radio,lcolesfera, false);


    Pixel rcolesfera = {0, 0, 1}; //azul
    Point rcentro(0.5, -0.7, -0.25);
    Sphere resfera(rcentro, radio,rcolesfera,false);

    //Definir elpunto de luz
    Point centro(0, 0.5, 0);
    Pixel power = {1,1,1};
    Light luz(centro, power);
    
    Primitive* primitivas[7];
    primitivas[0] = &planol;
    primitivas[1] = &planor;
    primitivas[2] = &planof;
    primitivas[3] = &planoc;
    primitivas[4] = &planob;
    primitivas[5] = &resfera;
    primitivas[6] = &lesfera;


    int n_primitivas = 7;

    Light* luces[1];
    luces[0] = &luz;
    int n_luces = 1;
    std::cout << "antes" << std::endl;
    //Image img = camara.generateImageWithLightingMultipleRaysBounces(primitivas, n_primitivas, luces, n_luces, 1, 64);
    //Image img2 = camara.generateImageWithLightingOneRay(primitivas, n_primitivas, luces, n_luces);
    std::cout << "antes" << std::endl;

    //img.clampPixels();
    //img.curvaGamma(0.5);
    //img.writePPM("imagenLuzRebotePrueba2.ppm");
    //img2.writePPM("imagenLuz1Rayo.ppm");
    std::cout << "Imagen luz  con rebote generada" << std::endl;   
    return 0;
}