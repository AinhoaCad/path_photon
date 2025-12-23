#include <vector>
#include <iostream>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Light.hpp"
#include "Renderizador.hpp"
#include "PhotonMapper.hpp"
#include <ctime>
#include <cstdlib>

// -----------------------------------------------------------------------------
// Declaraciones de Funciones Externas
// -----------------------------------------------------------------------------
extern float randomOffset(float pixelSize);
extern Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives);
extern InterseccionData calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives);

extern Pixel calcularIluminacionGlobalConFotones(
    const InterseccionData& inter, 
    const std::vector<Light>& lights, 
    const std::vector<Primitive*>& primitives, 
    const Direction& wo,
    PhotonMapper* photonMapper, 
    float pm_radius, 
    int pm_nPhotons,
    KERNEL pm_kernelType,
    int depth, 
    int maxBounces
);

// -----------------------------------------------------------------------------
// IMPLEMENTACIÓN DE LA CLASE CAMERA
// -----------------------------------------------------------------------------
Camera::Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp)
    : origen(origen), left(l), up(u), forward(f), pixelWidth(anchp), pixelHeight(altp) {
    float anchoPlano = 2.0f;
    float altoPlano = 2.0f;

    onepixelWidth = anchoPlano / pixelWidth;
    onepixelHeight = altoPlano / pixelHeight;
}

Ray Camera::generateRay(int i, int j, bool multipleSamples) {
    float anchoPlano = 2.0f;
    float altoPlano = 2.0f;

    // Coordenadas locales de pixel
    float pixelX = -(j + 0.5f) * onepixelWidth + anchoPlano / 2.0f; 
    float pixelY = -(i + 0.5f) * onepixelHeight + altoPlano / 2.0f;

    if (multipleSamples) {
        pixelX += randomOffset(onepixelWidth);
        pixelY += randomOffset(onepixelHeight);
    }

    Direction rayDirLocal(pixelX, pixelY, 1.0f);
    rayDirLocal = Operations::norm(rayDirLocal);

    // Transformación de cámara a mundo
    std::vector<Matrix> matrices = {
        up.getHomMatrix(),
        left.getHomMatrix(),
        forward.getHomMatrix(),
        origen.getHomMatrix()
    };
    Matrix cameraToWorld(matrices); 
    Matrix rayHom = cameraToWorld * rayDirLocal.getHomMatrix(); 
    Direction rayDirWorld(rayHom);

    return Ray(origen.toVector(), rayDirWorld.toVector());
}

Image Camera::generateImage(const std::vector<Primitive*>& primitives) {
    Image image(pixelWidth, pixelHeight);

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Ray ray = generateRay(y, x, false); 
            Pixel pixelColor = calcularColorMasCercano(ray, primitives); 
            image.setPixel(y, x, pixelColor);
        }
    }

    return image;
}

Image Camera::generateImageWithLightingMultipleRaysConFotones(
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    PhotonMapper* photonMapper,
    int numRays,
    int maxBounces,
    float pm_radius,
    int pm_nPhotons,
    KERNEL pm_kernelType
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(nullptr)));

    if (!photonMapper) {
        std::cerr << "Advertencia: PhotonMapper nulo. Renderizando solo luz directa." << std::endl;
    }

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Pixel acumulado = {0, 0, 0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(y, x, true);
                InterseccionData inter = calcularInterseccionMasCercana(ray, primitives);

                Pixel color = {0, 0, 0};
                if (inter.hayInterseccion && inter.primitiva) {
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    // shade (intersection)
                    color = calcularIluminacionGlobalConFotones(
                        inter, lights, primitives, wo,
                        photonMapper, pm_radius, pm_nPhotons,
                        pm_kernelType,
                        0, maxBounces
                    );
                }

                acumulado.r += color.r;
                acumulado.g += color.g;
                acumulado.b += color.b;
            }

            acumulado.r /= numRays;
            acumulado.g /= numRays;
            acumulado.b /= numRays;

            image.setPixel(y, x, acumulado);
        }
    }

    return image;
}


