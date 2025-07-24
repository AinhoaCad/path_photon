#include <iostream>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

Scene createCornellBox() {
    Scene scene;

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
    scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    std::cout << "Escena creada." << std::endl;

    return scene;
}


Camera CameraConfiguration() {

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
    Camera camera = CameraConfiguration();
    
    // Generar imagen
    Image image = camera.generateImage(scene.getPrimitives());

    int numRays = 64; // Número de rayos por píxel
    Image image1 = camera.generateImageMultipleRays(scene.getPrimitives(),numRays);

    //Generar imagen con luz
    Image image2 = camera.generateImageWithLightingOneRay(scene.getPrimitives(), scene.getLights());
    Image image3 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);

    image.writePPM("imagen1Rayo.ppm");
    image1.writePPM("imagenMultiplesRayos.ppm");
    image2.writePPM("imagenLuz1Rayo.ppm");
    image3.writePPM("imagenLuzMultiplesRayos.ppm");

    //Generar imagen con aplicaciones
    Image image4 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);

    image4.clampPixels();
    image4.curvaGamma(0.5);

    image4.writePPM("imagenLuzMultiplesRayosModificaciones.ppm");

    //img2.writePPM("imagenLuz1Rayo.ppm");
    std::cout << "Imagen luz  con rebote generada" << std::endl;   
    return 0;
    
}
    