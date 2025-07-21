#include <iostream>
#include "Scene.hpp"

Scene createCornellBox() {
    Scene scene;

    // Materiales
    Material redDiffuse(Color(1.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material greenDiffuse(Color(0.0f, 1.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material whiteDiffuse(Color(0.8f, 0.8f, 0.8f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);

    // Paredes como planos
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));   // Izquierda
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), -1.0f, greenDiffuse)); // Derecha
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, whiteDiffuse)); // Piso
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), -1.0f, whiteDiffuse)); // Techo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, 1.0f), 3.0f, whiteDiffuse));  // Fondo

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, whiteDiffuse));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, whiteDiffuse));
    
    // Luz puntual
    scene.addLight(PointLight(Vector3(0.0f, 0.9f, -2.0f), Color(10.0f, 10.0f, 10.0f)));

    std::cout << "Escena creada." << std::endl;

    return scene;
}

/*
void testCameraConfiguration() {
    Camera camera;
    camera.origen = Vector3(0.0f, 0.0f, 0.0f);
    camera.l = Vector3(-1.0f, 0.0f, 0.0f);
    camera.u = Vector3(0.0f, 1.0f, 0.0f);
    camera.f = Vector3(0.0f, 0.0f, -1.0f);
    camera.pixelWidth = 256;
    camera.pixelHeight = 256;
    camera.onepixelWidth = camera.anchoPlano / camera.pixelWidth;
    camera.onepixelHeight = camera.altoPlano / camera.pixelHeight;

    assert(camera.origen == Point(0.0f, 0.0f, 0.0f));
    assert(camera.left == Direction(-1.0f, 0.0f, 0.0f));
    assert(camera.up == Direction(0.0f, 1.0f, 0.0f));
    assert(camera.forward == Direction(0.0f, 0.0f, -1.0f));
    assert(camera.pixelWidth == 256);
    assert(camera.pixelHeight == 256);
    assert(camera.onepixelWidth == (camera.anchoPlano / camera.pixelWidth));
    assert(camera.onepixelHeight == (camera.altoPlano / camera.pixelHeight));

    std::cout << "Configuración de la cámara verificada correctamente." << std::endl;

    //--------------------------------------------------------------------------------
    
    //Definir la camara
    Point origen(0, 0, -3.5);
    Direction l(-1, 0, 0);
    Direction u(0, 1, 0);
    Direction f(0, 0, 3);
    Camera camara(origen, l, u, f, 256, 256);
}
    */