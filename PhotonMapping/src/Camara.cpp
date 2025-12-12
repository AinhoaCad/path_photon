#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Light.hpp"
#include "Plane.hpp"
#include "Renderizador.hpp"
#include <ctime> // Para usar srand
#include <cstdlib> // Para generar números aleatorios


Camera::Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp)
    : origen(origen), left(l), up(u), forward(f), pixelWidth(anchp), pixelHeight(altp) {
    // Calcular dimensiones físicas del plano de la imagen
    onepixelWidth = anchoPlano / pixelWidth;
    onepixelHeight = altoPlano / pixelHeight;
}

// Genera un rayo desde la cámara hacia un píxel
Ray Camera::generateRay(int i, int j, bool multipleSamples) {

    // -------- GENERACIÓN DE RAYO EN COORDENADAS DE LA CÁMARA --------
    // En el sistema de cámara:
    // - El origen de todos los rayos es (0, 0, 0).
    // - Cada rayo apunta hacia un punto distinto del plano de imagen (pixelX, pixelY, 1).

    // Calcular el centro del píxel (i, j) en coordenadas de la cámara
    float pixelX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2.0f;
    float pixelY = -(j + 0.5f) * onepixelHeight + altoPlano / 2.0f;

    // Si se activan múltiples muestras por píxel añadimos un pequeño desplazamiento aleatorio dentro del área del píxel
    if (multipleSamples) {
        pixelX += randomOffset(onepixelWidth);
        pixelY += randomOffset(onepixelHeight);
    }

    // Dirección del rayo en coordenadas locales de cámara:
    // (desde el origen (0,0,0) hasta el punto del plano (pixelX, pixelY, 1))
    //EN COORDENADAS LOCALES DE CÁMARA
    Direction rayDirLocal(pixelX, pixelY, 1.0f);

    // Normalizamos la direccion
    rayDirLocal = Operations::norm(rayDirLocal); 


    // -------- TRANSFORMACIÓN A COORDENADAS GLOBALES--------
    // La cámara tiene su propio sistema de referencia (left, up, forward, origen).
    // Creamos la matriz que transforma de espacio de cámara → espacio global.
    std::vector<Matrix> matrices = {
        left.getHomMatrix(),
        up.getHomMatrix(),
        forward.getHomMatrix(),
        origen.getHomMatrix()
    };
    Matrix cameraToWorld(matrices); 

    // Transformamos la dirección local al sistema de coordenadas global.
    Matrix rayHom = cameraToWorld * rayDirLocal.getHomMatrix();
    Direction rayDirWorld(rayHom);

    // -------- CONSTRUCCIÓN DEL RAYO FINAL --------
    // El rayo se define por:
    // - Origen: posición de la cámara en coordenadas globales.
    // - Dirección: vector rayDirWorld, que está en coordenadas globales.
    return Ray(origen.toVector(), rayDirWorld.toVector());
}


// ------------------------------------------------------------------------------------------------
// Función de Renderizado usando Photon Mapping para la Iluminación Global (Indirecta)
// ------------------------------------------------------------------------------------------------
Image Camera::generateImageWithLightingMultipleRaysConFotones(
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    PhotonMapper* photonMapper, // <-- ¡Nuevo parámetro!
    int numRays,
    int maxBounces,
    float pm_radius,           // Radio de búsqueda para PM
    int pm_nPhotons            // N fotones a buscar
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(nullptr)));

    // Validación del PhotonMapper y Mapa
    if (!photonMapper) {
        std::cerr << "Advertencia: PhotonMapper nulo o vacío. Renderizando solo luz directa." << std::endl;
    }

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Pixel acumulado = {0,0,0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(y, x, true);
                Interseccion inter = calcularInterseccionMasCercana(ray, primitives);
                Pixel color = {0,0,0};

                if (inter.hayInterseccion && inter.primitiva) {

                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    // Calcular iluminación global usando la función con Photon Mapping
                    color = calcularIluminacionGlobalConFotones(
                        inter, lights, primitives, wo, 
                        photonMapper, pm_radius, pm_nPhotons, 
                        0, maxBounces
                    );
                }

                acumulado.r += color.r;
                acumulado.g += color.g;
                acumulado.b += color.b;
            }
            
            // Promediar color de todos los rayos por píxel
            acumulado.r /= numRays;
            acumulado.g /= numRays;
            acumulado.b /= numRays;

            image.setPixel(y, x, acumulado);
        }
    }

    return image;
}

