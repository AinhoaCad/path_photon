#include <iostream>
#include <string>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "PhotonMapper.hpp"
#include "Photon.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Tonemapping.hpp"

/**
 * Crea, configura y construye el mapa de fotones para la escena.
 * @param scene La escena con primitivas y luces.
 * @param maxBounces Máximo de rebotes permitidos para los fotones.
 * @return PhotonMapper El objeto PhotonMapper ya construido.
 */


// PASS 1 : Emisión y construcción del mapa de fotones
PhotonMapper createAndBuildPhotonMapper(Scene& scene, int maxBounces) {

    const int N_PHOTONS_TO_SHOOT = 300000;  // Número total de fotones a emitir
    const int PM_MAX_BOUNCES = maxBounces;

    std::cout << "\n--- Construyendo Photon Map (Fase 1: Emision) ---\n";
    std::cout << "   Fotones a emitir = " << N_PHOTONS_TO_SHOOT << "\n"
              << "   Rebotes max = " << PM_MAX_BOUNCES << "\n";

    PhotonMapper photonMapper(&scene); //all_photons[]
    // Lanzamos los fotones y hacemos el tracing
    photonMapper.emitPhotons(N_PHOTONS_TO_SHOOT);

    photonMapper.buildPhotonMap();

    std::cout << "   Fotones almacenados en KD-Tree: " 
              << photonMapper.getTotalStoredPhotons() << "\n";

    return photonMapper;
}

// --------------------------------------------------------------
// ESCENAS
// --------------------------------------------------------------

Scene createCornellBox() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, greenDiffuse));
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), 1.0f, grayDiffuse));
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, grayDiffuse));
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, -1.0f), 1.0f, grayDiffuse));

    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25f), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25f), 0.3f, cyanDiffuse, false));

    scene.addLight(Light(Vector3(0.0f, 0.5f, 0.0f), Color(1, 1, 1).toPixel()));

    std::cout << "Escena Cornell Box creada.\n";

    return scene;
}

Scene createCornellBoxLuzTecho() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1, 0, 0), 1, redDiffuse));
    scene.addPlane(Plane(Vector3(-1, 0, 0), 1, greenDiffuse));
    scene.addPlane(Plane(Vector3(0, 1, 0), 1, grayDiffuse));

    Light techoLight(
        Point(0, 1, 0),
        Vector3(0, -1, 0),
        2.0f,
        2.0f,
        Pixel{5, 5, 5}
    );

    scene.addPlane(Plane(Vector3(0, -1, 0), 1, grayDiffuse, techoLight));

    scene.addPlane(Plane(Vector3(0, 0, -1), 1, grayDiffuse));

    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25f), 0.3f, purpleDiffuse, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25f), 0.3f, cyanDiffuse, false));

    std::cout << "Escena Cornell Box con techo emitiendo luz creada.\n";

    return scene;
}

Scene createCornellBoxMaterialesRaros() {
    Scene scene;

    scene.addPlane(Plane(Vector3(1, 0, 0), 1, redDiffuse));
    scene.addPlane(Plane(Vector3(-1, 0, 0), 1, greenDiffuse));
    scene.addPlane(Plane(Vector3(0, 1, 0), 1, grayDiffuse));

    Light techoLight(
        Point(0, 1, 0),
        Vector3(0, -1, 0),
        2.0f,
        2.0f,
        Pixel{1, 1, 1}
    );
    scene.addPlane(Plane(Vector3(0, -1, 0), 1, grayDiffuse, techoLight));

    scene.addPlane(Plane(Vector3(0, 0, -1), 1, grayDiffuse));

    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25f), 0.3f, mirrorSpecular, false));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25f), 0.3f, glassDielectric, false));

    std::cout << "Escena Cornell Box con materiales raros creada.\n";

    return scene;
}

Camera CameraConfiguration(int width, int height) {
    Point origen(0, 0, -3.5f);
    Direction l(-1, 0, 0);
    Direction u(0, 1, 0);
    Direction f(0, 0, 3);
    Camera camara(origen, l, u, f, width, height);
    return camara;
}

// --------------------------------------------------------------
// MAIN
// --------------------------------------------------------------

int main(int argc, char** argv) {

    int maxBounces = 12;
    Scene scene;

    // Parámetros para Photon Mapping

    const int PM_N_PHOTONS_SEARCH = 300; // k-nearest photons
    const float PM_MAX_RADIUS = 0.05f; // Radio maximo de búsqueda
    const KERNEL PM_KERNEL_TYPE = BOX; // Tipo de kernel

    if (argc != 5) {
        std::cout << "Uso: " << argv[0]
                  << " <muestras_por_pixel> <ancho> <alto> <archivo_salida>\n";
        return 1;
    }

    int numRays = std::stoi(argv[1]);
    int width = std::stoi(argv[2]);
    int height = std::stoi(argv[3]);
    std::string outputFile = argv[4];

    if (numRays <= 0 || width <= 0 || height <= 0) {
        std::cerr << "ERROR: Los parametros deben ser > 0.\n";
        return 1;
    }

    std::cout << "Renderizando con:\n"
              << "  SPP = " << numRays << "\n"
              << "  Resolucion = " << width << " x " << height << "\n"
              << "  Archivo = " << outputFile << "\n";

    scene = createCornellBox();

    Camera camera = CameraConfiguration(width, height);

    // PASS 1: Crear y construir el mapa de fotones

    PhotonMapper photonMapper = createAndBuildPhotonMapper(scene, maxBounces);

    std::cout << "\n--- Renderizando con Photon Mapping ---\n";
    std::cout << "  Fotones a buscar = " << PM_N_PHOTONS_SEARCH << "\n";
    std::cout << "  Radio max = " << PM_MAX_RADIUS << "\n";
    std::cout << "  Kernel = " 
              << (PM_KERNEL_TYPE == CONE ? "Conico" : (PM_KERNEL_TYPE == GAUSSIAN ? "Gaussiano" : "Caja"))
              << "\n";


    // PASS 2: Renderizado usando el mapa de fotones
    Image image = camera.generateImageWithLightingMultipleRaysConFotones(
        scene.getPrimitives(),
        scene.getLights(),
        &photonMapper,
        numRays,
        maxBounces,
        PM_MAX_RADIUS,
        PM_N_PHOTONS_SEARCH,
        PM_KERNEL_TYPE
    );

    // Aplicar tone mapping
    Tonemapping::applyToneMapping(image, ToneMappingOperator::GAMMA, 1.0f / 1.5f);

    // Build informative filename suffix: <base>_<Kernel>M<M>N<N>R<R>
    // Kernel: Box | Cone | Gaussian
    // Radius: print with two decimals (e.g., R1.00)
    std::string kernelName = (PM_KERNEL_TYPE == BOX ? "Box" : (PM_KERNEL_TYPE == CONE ? "Cone" : "Gaussian"));
    char suffix[64];
    snprintf(suffix, sizeof(suffix), "_%sN%dR%.2f", kernelName.c_str(), PM_N_PHOTONS_SEARCH, PM_MAX_RADIUS);
    // Insert suffix before extension
    std::string pm_name = "pm_" + outputFile;
    size_t dot = pm_name.find_last_of('.');
    if (dot != std::string::npos) {
        pm_name.insert(dot, suffix);
    } else {
        pm_name += suffix;
    }
    
    // Usar el sistema de write() para elegir formato automáticamente
    if (image.write(pm_name)) {
        std::cout << "Imagen guardada en: " << pm_name << std::endl;
    } else {
        std::cerr << "Error al guardar la imagen: " << pm_name << std::endl;
    }

    std::cout << "IMAGENES GENERADAS\n";
    return 0;
}

    