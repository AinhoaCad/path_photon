#include <iostream>
#include <chrono>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Triangle.hpp"
#include "SceneBVH.hpp"
#ifdef USE_EMBREE
#include "SceneEmbree.hpp"
#endif
#include "OBJLoader.hpp"
#include "Tonemapping.hpp"


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

// Cornell Box diseñada específicamente para ver cáusticas claras
Scene createCornellBoxCausticas() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));    // izquierda
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse)); // derecha
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));   // suelo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));  // fondo

    // Luz de área INTENSA y pequeña en el techo (simula luz puntual direccional)
    Light techoLight(
        Point(0.0f, 0.95f, 0.0f),    // centro de la luz
        Vector3(0.0f, -1.0f, 0.0f),  // normal hacia abajo
        0.3f,                         // ancho pequeño (luz más concentrada)
        0.3f,                         // alto pequeño
        Pixel{1.0f, 1.0f, 1.0f}      // Intensidad moderada para no sobreexponer
    );
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse, techoLight)); // Techo con luz

    // Esfera de cristal GRANDE flotando - actuará como lente
    // Centrada, a media altura para que las cáusticas lleguen al suelo
    scene.addSphere(Sphere(Vector3(0.3f, -0.6f, 0.0f), 0.35f, glassDielectric, false));


    return scene;
}

Scene createCornellBoxFlotante() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));    // izquierda
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse)); // derecha
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));   // suelo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));  // fondo
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse)); // Techo

    // Esfera de cristal difusa flotando 
    scene.addSphere(Sphere(Vector3(0.0f, -0.1f, 0.0f), 0.3f, yellowDiffuse, false));

    // Luz puntual
    scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

    std::cout << "Escena del Cornell Box con esfera flotante creada." << std::endl;

    return scene;
}

Scene createCornellBoxFlotanteArea() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));    // izquierda
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse)); // derecha
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));   // suelo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));  // fondo

    // Esfera de cristal difusa flotando 
    scene.addSphere(Sphere(Vector3(0.0f, -0.1f, 0.0f), 0.3f, yellowDiffuse, false));

    // Luz de área INTENSA y pequeña en el techo (simula luz puntual direccional)
    Light techoLight(
        Point(0.0f, 0.95f, 0.0f),    // centro de la luz
        Vector3(0.0f, -1.0f, 0.0f),  // normal hacia abajo
        0.3f,                         // ancho pequeño (luz más concentrada)
        0.3f,                         // alto pequeño
        Pixel{1.0f, 1.0f, 1.0f}      // Intensidad moderada para no sobreexponer
    );
    scene.addPlane(Plane(Vector3(0.0f, -0.7f, 0.0f), 1.0f, grayDiffuse, techoLight)); // Techo con luz

    std::cout << "Escena del Cornell Box con esfera flotante creada." << std::endl;

    return scene;
}

Scene createCornellBoxMaterialesRaros2() {
    Scene scene;

    // Pared izquierda (rojo), x = -1
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));

    // Pared derecha (verde), x = 1
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse));

    // Suelo, y = -1
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));

    
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse)); // Techo con emisión
    //La luz en los planos se añade automáticamente al vector de luces en addPlane, asi que no es necesario añadirla manualmente con addLigh

    // Fondo, z = 1
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, plasticMaterial, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, shinyGold, false));
    
    // Luz puntual
    scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1.0f, 1.0f, 1.0f).toPixel()));

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
    Scene CornellBoxBasicaFlotante = createCornellBoxFlotante();
    Scene CornellBoxLuzTecho = createCornellBoxLuzTecho(); 
    Scene CornellBoxMaterialesRaros = createCornellBoxMaterialesRaros();
    Scene CornellBoxConMalla = createCornellBoxConMalla();
    Scene CornellBoxConMallaOBJ = createCornellBoxConMallaOBJ("templo_compuesto.obj");
    Scene CornellBoxCausticas = createCornellBoxCausticas();
    Scene CornellBoxFlotanteArea = createCornellBoxFlotanteArea();
    Scene CornellBoxMaterialesRaros2 = createCornellBoxMaterialesRaros2();

    scene = CornellBoxCausticas; // Elegir la escena a renderizar aquí
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

    
    //--------------------------------------------------------------
    // IMAGEN 6: Séptima imagen con BVH, luz, rebotes y modificaciones y BVH MANUAL
    //--------------------------------------------------------------
    /*
    std::cout << "\n[BVH] Creando escena con aceleración BVH...\n";
    
    // Crear instancia de SceneBVH con la misma escena
    SceneBVH sceneBVH(scene);
    
    std::cout << "[BVH] Construyendo imagen con BVH...\n";
    
    // Generar imagen usando BVH
    auto startBVH = std::chrono::steady_clock::now();

    Image imageBVH = camera.generateImageWithLightingBVH(
        sceneBVH,
        scene.getLights(),
        numRays,
        maxBounces
    );

    auto endBVH = std::chrono::steady_clock::now();
    
    // Aplicar las mismas modificaciones
    Tonemapping::clampPixels(imageBVH);
    Tonemapping::curvaGamma(imageBVH, 1.0f/1.5f);
    
    // Guardar con sufijo "_BVH" para diferenciar
    std::string outputFileBVH = outputFile;
    size_t dotPos = outputFileBVH.find_last_of(".");
    if (dotPos != std::string::npos) {
        outputFileBVH.insert(dotPos, "_BVH");
    } else {
        outputFileBVH += "_BVH";
    }
    
    imageBVH.write(outputFileBVH);
    auto bvhMs = std::chrono::duration_cast<std::chrono::milliseconds>(endBVH - startBVH).count();
    std::cout << "[T] Path tracing con BVH: " << bvhMs / 1000.0 << " s\n";
    std::cout << "[BVH] Imagen guardada en: " << outputFileBVH << "\n";
    */

    /*

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
*/


    //--------------------------------------------------------------
    // Mostrar mensaje de finalización
    //--------------------------------------------------------------
    std::cout << "IMAGENES GENERADAS" << std::endl;   
    return 0;
    
}
    