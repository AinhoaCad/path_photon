#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Light.hpp"
#include "Plane.hpp"
#include "Renderizador.hpp"
#include "SceneBVH.hpp"
#ifdef USE_EMBREE
#include "SceneEmbree.hpp"
#endif
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

// IMAGEN 0: Primera imagen sin luz y un rayo por pixel
Image Camera::generateImage(const std::vector<Primitive*>& primitives) {
    Image image(pixelWidth, pixelHeight);

    // Recorremos la imagen píxel por píxel
    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {

            // Generar el rayo que pasa por el centro del píxel (un rayo por píxel)
            Ray ray = generateRay(y, x, false);

            // Calcular el color del objeto más cercano que intersecta el rayo
            Pixel pixelColor = calcularColorMasCercano(ray, primitives);

            // Ese pixel sera de ese color
            image.setPixel(y, x, pixelColor);
        }
    }

    return image;
}

// IMAGEN 1: Segunda imagen con múltiples rayos por pixel
Image Camera::generateImageMultipleRays(const std::vector<Primitive*>& primitives, int numRays) {

    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(NULL))); // Semilla aleatoria

    // Recorremos todos los píxeles de la imagen
    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {

            std::vector<Pixel> muestras; // almacenará los colores de cada rayo

            // lanzamos varios rayos por píxel
            for (int r = 0; r < numRays; ++r) {
                // Generar un rayo con desplazamiento aleatorio dentro del píxel
                Ray ray = generateRay(y, x, true);

                // Calcular el color de la intersección más cercana
                Pixel color = calcularColorMasCercano(ray, primitives);

                // Guardar la muestra
                muestras.push_back(color);
            }

            // Promediar el color final del píxel
            Pixel colorPromedio = promedioColores(muestras);

            // Asignar el color resultante al píxel de la imagen
            image.setPixel(y, x, colorPromedio);
        }
    }

    return image;
}

// IMAGEN 2: Tercera imagen con luz y un rayo por pixel
Image Camera::generateImageWithLightingOneRay(const std::vector<Primitive*>& primitives, const std::vector<Light>& luces) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(NULL))); // Inicializa generador de numeros aleatorios

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            
            // Generar un solo rayo desde la cámara hacia el píxel
            Ray ray = generateRay(y, x, false);

            // Calcular intersección más cercana con la escena
            Interseccion inter = calcularInterseccionMasCercana(ray, primitives);

            // Si hay impacto, calcular iluminación
            Pixel color = {0, 0, 0};
            if (inter.hayInterseccion && inter.primitiva) {
                color = calcularIluminacion(inter, luces, primitives);
            }

            // Dibujar píxel
            image.setPixel(y, x, color);
        }
    }

    return image;
}

// IMAGEN 3: Cuarta imagen con luz y múltiples rayos por pixel
Image Camera::generateImageWithLightingMultipleRays(const std::vector<Primitive*>& primitives, const std::vector<Light>& luces, int numRays) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(NULL)));

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {

            std::vector<Pixel> coloresRayos;

            // --- Para cada subrayo ---
            for (int r = 0; r < numRays; ++r) {

                // Generar rayo aleatorio dentro del píxel
                Ray ray = generateRay(y, x, true);

                // Calcular la intersección más cercana con la escena
                Interseccion inter = calcularInterseccionMasCercana(ray, primitives);

                // Si hay impacto, calcular la iluminación en ese punto
                Pixel colorRayo = {0, 0, 0};
                if (inter.hayInterseccion && inter.primitiva) {
                    colorRayo = calcularIluminacion(inter, luces, primitives);
                }

                coloresRayos.push_back(colorRayo);
            }

            // Promediar todos los colores de los rayos usando función auxiliar
            Pixel colorFinal = promedioColores(coloresRayos);

            image.setPixel(y, x, colorFinal);
        }
    }

    return image;
}


// ------------- generateImageWithLightingMultipleRaysConRebotes -------------

Image Camera::generateImageWithLightingMultipleRaysConRebotes(
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    int numRays,
    int maxBounces
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(nullptr)));

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Pixel acumulado = {0,0,0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(y, x, true);
                Interseccion inter = calcularInterseccionMasCercana(ray, primitives);
                Pixel color = {0,0,0};

                if (inter.hayInterseccion && inter.primitiva) {

                    //Direccion hacia la camara desde el punto de impacto
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    // Calcular iluminación global, incluyendo rebotes indirectos
                    color = calcularIluminacionGlobal(inter, lights, primitives, wo, true, 0, maxBounces);
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


// ============= NUEVO: generateImageWithLightingBVH =============
// Versión acelerada con BVH, sin tocar el código anterior

Image Camera::generateImageWithLightingBVH(
    const SceneBVH& sceneBVH,
    const std::vector<Light>& lights,
    int numRays,
    int maxBounces
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(nullptr)));

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Pixel acumulado = {0, 0, 0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(y, x, true);
                
                // ✅ CAMBIO PRINCIPAL: Usar BVH en lugar de búsqueda lineal
                Interseccion inter = sceneBVH.intersect(ray);
                
                Pixel color = {0, 0, 0};

                if (inter.hayInterseccion && inter.primitiva) {
                    // Dirección hacia la cámara desde el punto de impacto
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    // Calcular iluminación global CON BVH
                    color = calcularIluminacionGlobalBVH(
                        inter, 
                        lights, 
                        sceneBVH, 
                        wo, 
                        true, 
                        0, 
                        maxBounces
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

#ifdef USE_EMBREE
// Versión acelerada con Embree (triángulos) + fallback analítico
Image Camera::generateImageWithLightingEmbree(
    const SceneEmbree& sceneEmbree,
    const std::vector<Light>& lights,
    int numRays,
    int maxBounces
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(nullptr)));

    for (int y = 0; y < pixelHeight; ++y) {
        for (int x = 0; x < pixelWidth; ++x) {
            Pixel acumulado = {0, 0, 0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(y, x, true);

                Interseccion inter = sceneEmbree.intersect(ray);

                Pixel color = {0, 0, 0};

                if (inter.hayInterseccion && inter.primitiva) {
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    color = calcularIluminacionGlobalEmbree(
                        inter,
                        lights,
                        sceneEmbree,
                        wo,
                        true,
                        0,
                        maxBounces
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
#endif


