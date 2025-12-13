#include <iostream>
#include <string>
#include "Scene.hpp"
#include "Camara.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "PhotonMapper.hpp"
#include "Photon.hpp"
#include "Light.hpp"

/**
 * Crea, configura y construye el mapa de fotones para la escena.
 * @param scene La escena con primitivas y luces.
 * @param maxBounces Máximo de rebotes permitidos para los fotones.
 * @return PhotonMapper El objeto PhotonMapper ya construido.
 */

PhotonMapper createAndBuildPhotonMapper(Scene& scene, int maxBounces, float amplificationM) {

    const int N_PHOTONS_TO_SHOOT = 100000;
    const float M = amplificationM;
    const int PM_MAX_BOUNCES = maxBounces;

    std::cout << "\n--- Construyendo Photon Map (Fase 1: Emision) ---\n";
    std::cout << "   Fotones a emitir = " << N_PHOTONS_TO_SHOOT << "\n"
              << "   Factor M (amplificacion) = " << M << "\n"
              << "   Rebotes max = " << PM_MAX_BOUNCES << "\n";

    PhotonMapper photonMapper(&scene);
    photonMapper.emitPhotons(N_PHOTONS_TO_SHOOT, M);

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

    const int PM_N_PHOTONS_SEARCH = 1000;
    const float PM_MAX_RADIUS = 0.1f;
    const KERNEL PM_KERNEL_TYPE = CONE;
    const float M = 100.0f; // Amplification factor for photon flux

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

    PhotonMapper photonMapper = createAndBuildPhotonMapper(scene, maxBounces, M);

    std::cout << "\n--- Renderizando con Photon Mapping ---\n";
    std::cout << "  Fotones a buscar = " << PM_N_PHOTONS_SEARCH << "\n";
    std::cout << "  Radio max = " << PM_MAX_RADIUS << "\n";
    std::cout << "  Kernel = " 
              << (PM_KERNEL_TYPE == CONE ? "Conico" : "Box") 
              << "\n";

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

    image.clampPixels();
    image.curvaGamma(1.0f / 1.5f);

    // Build informative filename suffix: <base>_<Kernel>M<M>N<N>R<R>
    // Kernel: Box | Cone | Gaussian
    // Radius: print with two decimals (e.g., R1.00)
    std::string kernelName = (PM_KERNEL_TYPE == BOX ? "Box" : (PM_KERNEL_TYPE == CONE ? "Cone" : "Gaussian"));
    char suffix[64];
    snprintf(suffix, sizeof(suffix), "_%sM%dN%dR%.2f", kernelName.c_str(), (int)M, PM_N_PHOTONS_SEARCH, PM_MAX_RADIUS);
    // Insert suffix before extension
    std::string pm_name = "pm_" + outputFile;
    size_t dot = pm_name.find_last_of('.');
    if (dot != std::string::npos) {
        pm_name.insert(dot, suffix);
    } else {
        pm_name += suffix;
    }
    // Ensure filename ends with .ppm
    if (pm_name.size() < 4 || pm_name.substr(pm_name.size() - 4) != ".ppm") {
        pm_name += ".ppm";
    }
    image.writePPM(pm_name);

    std::cout << "Imagen guardada en: " << pm_name << std::endl;

    std::cout << "IMAGENES GENERADAS\n";
    return 0;
}

    