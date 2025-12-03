#include <iostream>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

Scene createCornellBox() {
    Scene scene;

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));

    // Techo, y = 1
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse));

    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, cyanDiffuse, false));
    
    // Luz puntual
    scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    std::cout << "Escena del Cornell Box creada." << std::endl;

    return scene;
}

//Otra escena de la Cornell Box con techo emitiendo luz en vez de una luz puntual
Scene createCornellBoxLuzTecho() {
    Scene scene;

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));

    // Techo emitiendo luz
    // Techo como luz de área
    
    Light techoLight(
        Point(0.0f, 1.0f, 0.0f),    // centro de la luz
        Vector3(0.0f, -1.0f, 0.0f), // normal hacia abajo
        2.0f,                       // ancho
        2.0f,                       // alto
        Pixel{5.0f, 5.0f, 5.0f}  // intensidad/poder de la luz aumentado
    );
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse, techoLight)); // Techo con emisión
    //La luz en los planos se añade automáticamente al vector de luces en addPlane, asi que no es necesario añadirla manualmente con addLight
    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, cyanDiffuse, false));
    

    std::cout << "Escena del Cornell Box con techo emitiendo luz creada." << std::endl;

    return scene;
}

Scene createCornellBoxMaterialesRaros() {
    Scene scene;

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));

    Light techoLight(
        Point(0.0f, 1.0f, 0.0f),    // centro de la luz
        Vector3(0.0f, -1.0f, 0.0f), // normal hacia abajo
        2.0f,                       // ancho
        2.0f,                       // alto
        Pixel{1.0f, 1.0f, 1.0f}  // intensidad/poder de la luz aumentado
    );
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse, techoLight)); // Techo con emisión
    //La luz en los planos se añade automáticamente al vector de luces en addPlane, asi que no es necesario añadirla manualmente con addLigh

    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, mirrorSpecular, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, glassDielectric, false));
    
    // Luz puntual
    //scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    std::cout << "Escena del Cornell Box creada." << std::endl;

    return scene;
}



Camera CameraConfiguration(int width, int height) {

    //--------------------------------------------------------------------------------
    
    //Definir la camara
    Point origen(0, 0, -3.5);
    Direction l(-1, 0, 0);
    Direction u(0, 1, 0);
    Direction f(0, 0, 3);
    Camera camara(origen, l, u, f, width, height);

    return camara;
}

int main(int argc, char** argv) {

    //--------------------------------------------------------------
    // VARIABLES Y PARAMETROS DE RENDERIZADO
    //--------------------------------------------------------------

    //int numRays = 64; // Número de rayos por píxel por defecto
    int maxBounces = 12; // Número máximo de rebotes de luz
    Scene scene;

    //número de muestras, alto, ancho y nombre de archivo por línea de comandos
    if (argc != 5) {
        std::cout << "Uso: " << argv[0]
                  << " <muestras_por_pixel> <ancho> <alto> <archivo_salida>\n";
        std::cout << "Ejemplo: " << argv[0]
                  << " 128 512 512 salida.ppm\n";
        return 1;
    }

    //muestras por pixel
    int numRays = std::stoi(argv[1]);
    if (numRays <= 0) {
        std::cerr << "ERROR: El número de muestras debe ser mayor que cero.\n";
        return 1;
    }

    //ancho imagen
    int width = std::stoi(argv[2]);
    if (width <= 0) {
        std::cerr << "ERROR: El ancho debe ser mayor que cero.\n";
        return 1;
    }

    //alto imagen
    int height = std::stoi(argv[3]);
    if (height <= 0) {
        std::cerr << "ERROR: El alto debe ser mayor que cero.\n";
        return 1;
    }

    //nombre archivo
    std::string outputFile = argv[4];

    std::cout << "Renderizando con:\n"
              << "  SPP = " << numRays << "\n"
              << "  Resolución = " << width << " x " << height << "\n"
              << "  Archivo = " << outputFile << "\n";

    //--------------------------------------------------------------
    // ESCENAS Y CÁMARA
    //--------------------------------------------------------------

    // Crea y ajusta la escena
    Scene CornellBoxBasica = createCornellBox();
    Scene CornellBoxLuzTecho = createCornellBoxLuzTecho(); 
    Scene CornellBoxMaterialesRaros = createCornellBoxMaterialesRaros();

    scene = CornellBoxMaterialesRaros; // Escena principal para renderizar (4 esferas con materiales nuevos)

    // Configurar la cámara
    Camera camera = CameraConfiguration(width, height);

    
    //--------------------------------------------------------
    // IMAGEN 0: Primera imagen sin luz y un rayo por pixel
    //------------------------------------------------------ 

    //Image image0 = camera.generateImage(scene.getPrimitives());
    //image0.writePPM("imagen1Rayo.ppm");

    //--------------------------------------------------------
    // IMAGEN 1: Segunda imagen con múltiples rayos por pixel
    //------------------------------------------------------

    //Image image1 = camera.generateImageMultipleRays(scene.getPrimitives(),numRays);
    //image1.writePPM("imagenMultiplesRayos.ppm");


    //--------------------------------------------------------
    // IMAGEN 2: Tercera imagen con luz y un rayo por pixel
    //------------------------------------------------------

    //Image image2 = camera.generateImageWithLightingOneRay(scene.getPrimitives(), scene.getLights());
    //image2.writePPM("imagenLuz1Rayo.ppm");


    //--------------------------------------------------------
    // IMAGEN 3: Cuarta imagen con luz y múltiples rayos por pixel
    //------------------------------------------------------

    //Image image3 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);
    //image3.writePPM("imagenLuzMultiplesRayos.ppm");

    //--------------------------------------------------------
    // IMAGEN 4: Quinta imagen con luz, múltiples rayos y modificaciones
    //------------------------------------------------------

    //Image image4 = camera.generateImageWithLightingMultipleRays(scene.getPrimitives(), scene.getLights(), numRays);

    //Modificaciones 
    //image4.clampPixels();
    //image4.curvaGamma(1/2.2);

    //image4.writePPM("imagenLuzMultiplesRayosModificaciones.ppm");



    //--------------------------------------------------------
    // IMAGEN 5: Sexta imagen con luz, rebotes y modificaciones
    //------------------------------------------------------

    Image image5 = camera.generateImageWithLightingMultipleRaysConRebotes(scene.getPrimitives(), scene.getLights(), numRays, maxBounces);

    //Modificaciones
    image5.clampPixels();
    image5.curvaGamma(1.0f/1.5f);

    image5.writePPM(outputFile);


    //--------------------------------------------------------------
    // Mostrar mensaje de finalización
    //--------------------------------------------------------------
    std::cout << "IMAGENES GENERADAS" << std::endl;   
    return 0;
    
}
    