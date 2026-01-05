#include <iostream>
#include <chrono>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Triangle.hpp"
#ifdef USE_EMBREE
#include "SceneEmbree.hpp"
#endif
#include "OBJLoader.hpp"
#include "Tonemapping.hpp"

// Cornell Box con una malla simple (dos triángulos formando un quad inclinado)
Scene createCornellBoxConMalla() {
    Scene scene;

    // Paredes
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));    // izquierda
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse)); // derecha
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));   // suelo
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse));  // techo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));  // fondo

    // Luz puntual centrada
    scene.addLight(Light(Vector3(0.0f, 0.6f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    // Malla: quad inclinado
    Point p0(-0.4f, -0.7f, 0.25f);
    Point p1( 0.4f, -0.7f, 0.25f);
    Point p2( 0.4f, -0.25f, -0.2f);
    Point p3(-0.4f, -0.25f, -0.2f);

    Vector3 e1 = p1 - p0;
    Vector3 e2 = p2 - p0;
    Direction n = e1.cross(e2).normalized().toDirection();

    scene.addTriangle(Triangle(p0, p1, p2, n, n, n, purpleDiffuse));
    scene.addTriangle(Triangle(p0, p2, p3, n, n, n, plasticMaterial));

    std::cout << "Cornell Box con malla sencilla creada." << std::endl;
    return scene;
}

Scene createCornellBoxConMallaOBJ(const std::string& objFilePath) {
    Scene scene;

    // Paredes
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));    // izquierda
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse)); // derecha
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));   // suelo
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse));  // techo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));  // fondo

    // Luz puntual centrada
    scene.addLight(Light(Vector3(0.0f, 0.6f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    // Cargar malla desde archivo OBJ
    // Parámetros: archivo, material, escala, traslación
    bool success = OBJLoader::loadOBJ(
        objFilePath,
        scene,
        purpleDiffuse,                      // Material
        Vector3(0.5f, 0.5f, 0.5f),         // Escala
        Vector3(0.0f, -0.5f, 0.0f)         // Traslación
    );

    if (!success) {
        std::cerr << "[ESCENA] Advertencia: No se pudo cargar el archivo OBJ " << objFilePath << std::endl;
        std::cerr << "[ESCENA] La escena estará vacía salvo las paredes." << std::endl;
    }

    std::cout << "Cornell Box con malla OBJ creada." << std::endl;
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
    Scene CornellBoxConMalla = createCornellBoxConMalla();
    Scene CornellBoxConMallaOBJ = createCornellBoxConMallaOBJ("templo_compuesto.obj");
    

    scene = CornellBoxConMalla; // Elegir la escena a renderizar aquí
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
    
    auto startLinear = std::chrono::steady_clock::now();

    Image image5 = camera.generateImageWithLightingMultipleRaysConRebotes(
        scene.getPrimitives(),
        scene.getLights(),
        numRays,
        maxBounces
    );

    auto endLinear = std::chrono::steady_clock::now();

    //Modificaciones
    Tonemapping::clampPixels(image5);
    Tonemapping::curvaGamma(image5, 1.0f/1.5f);

    image5.write(outputFile);

    auto linearMs = std::chrono::duration_cast<std::chrono::milliseconds>(endLinear - startLinear).count();
    std::cout << "[T] Path tracing: " << linearMs / 1000.0 << " s\n";


#ifdef USE_EMBREE
    //--------------------------------------------------------------
    // IMAGEN 7: Opción con Embree (triángulos) + fallback analítico 
    //--------------------------------------------------------------
    std::cout << "\n[Embree] Creando escena con aceleración Embree...\n";

    SceneEmbree sceneEmbree(scene);

    std::cout << "[Embree] Construyendo imagen con Embree...\n";

    auto startEmbree = std::chrono::steady_clock::now();

    Image imageEmbree = camera.generateImageWithLightingEmbree(
        sceneEmbree,
        scene.getLights(),
        numRays,
        maxBounces
    );

    auto endEmbree = std::chrono::steady_clock::now();

    Tonemapping::clampPixels(imageEmbree);
    Tonemapping::curvaGamma(imageEmbree, 1.0f/1.5f);

    std::string outputFileEmbree = outputFile;
    size_t dotPosEmbree = outputFileEmbree.find_last_of(".");
    if (dotPosEmbree != std::string::npos) {
        outputFileEmbree.insert(dotPosEmbree, "_Embree");
    } else {
        outputFileEmbree += "_Embree";
    }

    imageEmbree.write(outputFileEmbree);
    auto embreeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endEmbree - startEmbree).count();
    std::cout << "[T] Path tracing con Embree: " << embreeMs / 1000.0 << " s\n";
    std::cout << "[Embree] Imagen guardada en: " << outputFileEmbree << "\n";
#endif



    //--------------------------------------------------------------
    // Mostrar mensaje de finalización
    //--------------------------------------------------------------
    std::cout << "IMAGENES GENERADAS" << std::endl;   
    return 0;
    
}
    