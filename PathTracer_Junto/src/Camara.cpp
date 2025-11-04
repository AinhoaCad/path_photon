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

Pixel calcularReboteIndirecto(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces
) {
    if (depth >= maxBounces) return {0, 0, 0};

    // --- 1️⃣ Iluminación directa ---
    Pixel colorDirecto = calcularIluminacionDirecta(origen, normal, material, wo, primitives, lights);

    // --- 2️⃣ Muestreo de nueva dirección según material ---
    Direction nextDir;
    Color weight;
    bool isDelta = false;
    muestrearDireccionRebote(material, normal, wo, nextDir, weight, isDelta);

    // --- 3️⃣ Lanza rayo de rebote ---
    Point nextOrigin = Operations::add(origen, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    Interseccion inter = calcularInterseccionMasCercana(nextRay, primitives);

    Pixel indirecto = {0, 0, 0};
    if (inter.hayInterseccion && inter.primitiva) {
        Direction newWo = Operations::mult(nextRay.direction_.toDirection(), -1.0f);
        Pixel rec = calcularReboteIndirecto(inter.punto, inter.normal, inter.primitiva->getMaterial(),
                                            newWo, primitives, lights, depth + 1, maxBounces);

        // Ponderar resultado recursivo
        indirecto.r = rec.r * weight.R;
        indirecto.g = rec.g * weight.G;
        indirecto.b = rec.b * weight.B;

        if (!isDelta) {
            indirecto.r *= 0.2f;
            indirecto.g *= 0.2f;
            indirecto.b *= 0.2f;
        }
    }

    // --- 4️⃣ Suma final ---
    Pixel total;
    total.r = colorDirecto.r + indirecto.r;
    total.g = colorDirecto.g + indirecto.g;
    total.b = colorDirecto.b + indirecto.b;

    return total;
}

// ------------- generateImageWithLightingMultipleRaysConRebotes (usa calcularIluminacion + rebote) -------------
Image Camera::generateImageWithLightingMultipleRaysConRebotes(
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    int numRays,
    int maxBounces
) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < pixelHeight; ++i) {
        for (int j = 0; j < pixelWidth; ++j) {
            Pixel acumulado = {0, 0, 0};

            for (int r = 0; r < numRays; ++r) {
                // 1️⃣ Generar rayo primario
                Ray ray = generateRay(i, j, true);

                // 2️⃣ Buscar intersección más cercana (usando auxiliar)
                Interseccion inter = calcularInterseccionMasCercana(ray, primitives);
                Pixel color = {0, 0, 0};

                if (inter.hayInterseccion && inter.primitiva) {
                    // 3️⃣ Si emite luz → añadir su radiancia
                    if (inter.primitiva->emitsLight()) {
                        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
                        Light areaLight = emisiva->getAreaLight();
                        Pixel emision = areaLight.calcularRadianciaRectangular();
                        color.r += emision.r;
                        color.g += emision.g;
                        color.b += emision.b;
                    }

                    // 4️⃣ Calcular iluminación directa básica (idéntica a tu versión original)
                    std::vector<bool> inShadow(lights.size(), false);
                    Pixel colorMat = inter.primitiva->getMaterial().toPixel();

                    for (size_t l = 0; l < lights.size(); ++l) {
                        if (estaEnSombra(inter.punto, inter.normal, lights[l], primitives)) continue;

                        Pixel contrib = {0, 0, 0};
                        if (lights[l].width > 0 && lights[l].height > 0)
                            contrib = lights[l].calcularLuzAreaSample(inter.punto, inter.normal, colorMat,
                                                                      lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX));
                        else
                            contrib = lights[l].calcularLuz(inter.punto, inter.normal, colorMat);

                        color.r += contrib.r;
                        color.g += contrib.g;
                        color.b += contrib.b;
                    }

                    // 5️⃣ Rebote indirecto (usa tu versión completa que va bien)
                    Material hitMat = inter.primitiva->getMaterial();
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);

                    Pixel indirect = calcularReboteIndirecto(inter.punto, inter.normal, hitMat, wo,
                                                             primitives, lights, 0, maxBounces);

                    color.r += indirect.r;
                    color.g += indirect.g;
                    color.b += indirect.b;
                }

                // 6️⃣ Acumular color del rayo
                acumulado.r += color.r;
                acumulado.g += color.g;
                acumulado.b += color.b;
            }

            // 7️⃣ Promediar por el número de rayos
            acumulado.r /= numRays;
            acumulado.g /= numRays;
            acumulado.b /= numRays;

            image.setPixel(i, j, acumulado);
        }
    }

    return image;
}




