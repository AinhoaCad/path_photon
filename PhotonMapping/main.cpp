#include <iostream>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

#include "PhotonMapper.hpp"
#include "Photon.hpp"


/**
 * @brief Crea, configura y construye el mapa de fotones para la escena.
 * * @param scene La escena con primitivas y luces.
 * @param maxBounces Máximo de rebotes permitidos para los fotones.
 * @return PhotonMapper El objeto PhotonMapper ya construido.
 */

// --- Función auxiliar corregida para coincidir con la implementación de PhotonMapper.cpp ---

PhotonMapper createAndBuildPhotonMapper(
    Scene& scene, 
    int maxBounces
) {
    // --------------------------------------------------------------
    // 1. PARÁMETROS DEL PHOTON MAPPER
    // --------------------------------------------------------------
    
    // Total de fotones a emitir (ajusta este valor)
    const int N_PHOTONS_TO_SHOOT = 1000000; 
    const int PM_MAX_BOUNCES = maxBounces; 
    
    std::cout << "\n--- 🔨 Construyendo Photon Map (Fase 1) ---\n";
    std::cout << "   Fotones a emitir = " << N_PHOTONS_TO_SHOOT << "\n"
              << "   Rebotes max. = " << PM_MAX_BOUNCES << "\n";
              
    // --------------------------------------------------------------
    // 2. CONSTRUCCIÓN (Inicialización y Emisión)
    // --------------------------------------------------------------
    
    // ¡CORPasar la dirección de la escena (Scene*) al constructor
    PhotonMapper photonMapper(&scene); 

    // Paso A: Emitir fotones y trazar sus caminos (guarda en photons_ internamente)
    // Nota: Suponemos que tu lógica interna usa PM_MAX_BOUNCES, aunque 'emitPhotons'
    // no lo recibe directamente, la variable global MAX_PHOTON_DEPTH parece controlarlo.
    // Si necesitas PM_MAX_BOUNCES, modifica 'emitPhotons' para recibirlo.
    photonMapper.emitPhotons(N_PHOTONS_TO_SHOOT);
    
    // Paso B: Construir el KD-Tree a partir de los fotones almacenados
    photonMapper.buildPhotonMap();
    
    return photonMapper;
}

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

    //Parámetros de Renderizado del Photon Mapping
    const int PM_N_PHOTONS_SEARCH = 50;     
    const float PM_MAX_RADIUS = 0.5f; 

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

    scene = CornellBoxBasica; // Escena principal para renderizar (4 esferas con materiales nuevos)

    // Configurar la cámara
    Camera camera = CameraConfiguration(width, height);

    //--------------------------------------------------------------
    // IMAGEN 6: Séptima imagen con Photon Mapping
    //--------------------------------------------------------------

    // 1. Fase de Construcción (Llamada a la función)
    // La escena (scene) debe ser pasada por referencia a la función.
    PhotonMapper photonMapper = createAndBuildPhotonMapper(scene, maxBounces);

    

    std::cout << "\n--- 🎨 Renderizando con Photon Mapping (Fase 2: Estimación) ---\n";
    std::cout << "   Fotones a buscar (N) = " << PM_N_PHOTONS_SEARCH << "\n";
    std::cout << "   Radio max. (R) = " << PM_MAX_RADIUS << "\n";


    Image image6 = camera.generateImageWithLightingMultipleRaysConFotones(
        scene.getPrimitives(), 
        scene.getLights(), 
        &photonMapper, // Pasamos el mapa por PUNTERO
        numRays, 
        maxBounces,
        PM_MAX_RADIUS,
        PM_N_PHOTONS_SEARCH
    );
    //Modificaciones
    //image6.clampPixels();
    //image6.curvaGamma(1.0f/1.5f);

    std::string pm_name = "pm_" + outputFile;
    image6.writePPM(pm_name);
    std::cout << "Imagen de Photon Mapping guardada en: " << pm_name << std::endl;


    //--------------------------------------------------------------
    // Mostrar mensaje de finalización
    //--------------------------------------------------------------
    std::cout << "IMAGENES GENERADAS" << std::endl;   
    return 0;
    
}
    