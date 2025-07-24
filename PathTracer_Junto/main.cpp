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

    std::cout << "Escena del Cornell Box creada." << std::endl;

    return scene;
}

//Otra escena de la Cornell Box con techo emitiendo luz en vez de una luz puntual
Scene createCornellBoxLuzTecho() {
    Scene scene;

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse, false));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse, false));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, blueDiffuse, false));

    // Techo emitiendo luz
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, blueDiffuse, true)); // Techo con emisión

    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, whiteDiffuse, false));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, grayDiffuse, false));
    
    // Luz puntual
    scene.addLight(Light(Vector3(0.5f, -1.5f, 0.5f), Color(1.5f, 1.5f, 1.5f).toPixel())); // Luz más intensa

    std::cout << "Escena del Cornell Box con techo emitiendo luz creada." << std::endl;

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

    //--------------------------------------------------------------
    // VARIABLES Y PARAMETROS DE RENDERIZADO
    //--------------------------------------------------------------

    int numRays = 64; // Número de rayos por píxel
    int maxBounces = 2; // Número máximo de rebotes de luz
    Scene scene;

    //--------------------------------------------------------------
    // ESCENAS Y CÁMARA
    //--------------------------------------------------------------

    // Crea y ajusta la escena
    Scene CornellBoxBasica = createCornellBox();
    Scene CornellBoxLuzTecho = createCornellBoxLuzTecho(); //Con este de momento no va

    scene = CornellBoxLuzTecho; // Escena principal para renderizar
    
    // Configurar la cámara
    Camera camera = CameraConfiguration();

    /*
    
    //--------------------------------------------------------
    // IMAGEN 0: Primera imagen sin luz y un rayo por pixel
    //------------------------------------------------------ 

    Image image0 = camera.generateImage(scene.getPrimitives());
    image0.writePPM("imagen1Rayo.ppm");

    //--------------------------------------------------------
    // IMAGEN 1: Segunda imagen con múltiples rayos por pixel
    //------------------------------------------------------

    Image image1 = camera.generateImageMultipleRays(scene.getPrimitives(),numRays);
    image1.writePPM("imagenMultiplesRayos.ppm");

    */

    //--------------------------------------------------------
    // IMAGEN 2: Tercera imagen con luz y un rayo por pixel
    //------------------------------------------------------

    Image image2 = camera.generateImageWithLightingOneRay(scene.getPrimitives(), scene.getLights());
    image2.writePPM("imagenLuz1Rayo.ppm");


    //--------------------------------------------------------
    // IMAGEN 3: Cuarta imagen con luz y múltiples rayos por pixel
    //------------------------------------------------------

    Image image3 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);
    image3.writePPM("imagenLuzMultiplesRayos.ppm");

    //--------------------------------------------------------
    // IMAGEN 4: Quinta imagen con luz, múltiples rayos y modificaciones
    //------------------------------------------------------

    Image image4 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);

    //Modificaciones 
    image4.clampPixels();
    image4.curvaGamma(0.5);

    image4.writePPM("imagenLuzMultiplesRayosModificaciones.ppm");



    //--------------------------------------------------------
    // IMAGEN 5: Sexta imagen con luz, rebotes y modificaciones
    //------------------------------------------------------

    Image image5 = camera.generateImageWithLightingMultipleRaysConRebotes(scene.getPrimitives(), scene.getLights(), numRays, maxBounces);

    //Modificaciones
    image5.clampPixels();
    image5.curvaGamma(0.5);

    image5.writePPM("imagenLuzRebotesModificaciones.ppm");


    //--------------------------------------------------------------
    // Mostrar mensaje de finalización
    //--------------------------------------------------------------
    std::cout << "IMAGENES GENERADAS" << std::endl;   
    return 0;
    
}
    