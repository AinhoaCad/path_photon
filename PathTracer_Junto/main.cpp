#include <iostream>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"

Scene createCornellBox() {
    Scene scene;

    // Materiales
    Material redDiffuse(Color(1.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material greenDiffuse(Color(0.0f, 1.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material whiteDiffuse(Color(0.8f, 0.8f, 0.8f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);

    // Paredes como planos
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, redDiffuse, false));   // Izquierda
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), -1.0f, greenDiffuse, false)); // Derecha
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, whiteDiffuse, false)); // Piso
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), -1.0f, whiteDiffuse, false)); // Techo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, 1.0f), 3.0f, whiteDiffuse, false));  // Fondo

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, whiteDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, whiteDiffuse, false));
    
    // Luz puntual
    scene.addLight(Light(Vector3(0.0f, 0.9f, -2.0f), Color(10.0f, 10.0f, 10.0f).toPixel()));

    std::cout << "Escena creada." << std::endl;

    return scene;
}


Camera testCameraConfiguration() {
    /*
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

    */

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
    