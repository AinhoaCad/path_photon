#include <iostream>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

Scene createCornellBox() {
    Scene scene;


    /*
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
    
    */

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse, false));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse, false));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, blueDiffuse, false));

    // Techo, y = 1
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, blueDiffuse, false));

    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, whiteDiffuse, false));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, grayDiffuse, false));
    
    // Luz puntual
    //scene.addLight(Light(Vector3(0.0f, 0.9f, -2.0f), Color(10.0f, 10.0f, 10.0f).toPixel()));

    std::cout << "Escena creada." << std::endl;

    return scene;
}


Camera testCameraConfiguration() {

    //--------------------------------------------------------------------------------
    
    //Definir la camara
    Point origen(0, 0, -3.5);
    Direction l(-1, 0, 0);
    Direction u(0, 1, 0);
    Direction f(0, 0, 3);
    Camera camara(origen, l, u, f, 256, 256);

    return camara;
}

int main() {

    // Crear la escena
    Scene scene = createCornellBox();
    
    // Configurar la cámara
    Camera camera = testCameraConfiguration();
    
    // Generar imagen
    Image image = camera.generateImage(scene.getPrimitives());

    int numRays = 64; // Número de rayos por píxel
    Image image1 = camera.generateImageMultipleRays(scene.getPrimitives(),numRays);

    //img.clampPixels();
    //img.curvaGamma(0.5);
    image.writePPM("imagenPrueba.ppm");
    image1.writePPM("imagenPrueba1.ppm");

    //img2.writePPM("imagenLuz1Rayo.ppm");
    std::cout << "Imagen luz  con rebote generada" << std::endl;   
    return 0;
    
}
    