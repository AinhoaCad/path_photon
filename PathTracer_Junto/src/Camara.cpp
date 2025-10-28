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

    // ------------------ Iluminación directa en este punto ------------------
    // Para materiales con lóbulo delta (especular o dieléctrico) la contribución directa
    // desde fuentes de área (como el techo) no debe contarse aquí, porque esos materiales
    // son muestreados como delta (reflejan/refractan en una dirección puntual).
    // Por tanto evitamos sumar iluminación directa cuando el material es delta.
    Pixel colorDirecto = {0, 0, 0};
    bool isDeltaMaterial = material.isSpecular() || material.isDielectric();
    if (!isDeltaMaterial) {
        for (size_t l = 0; l < lights.size(); ++l) {
            Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
            Point lightSample;
            Direction toLight;
            float dist;

            // Distinguir luz de área de luz puntual
            if (lights[l].width > 0 && lights[l].height > 0) {
                lightSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                toLight = Operations::sub(lightSample, shadowOrigin);
                dist = Operations::mod(toLight);
                toLight = Operations::norm(toLight);
            } else {
                lightSample = lights[l].origen;
                toLight = Operations::sub(lightSample, shadowOrigin);
                dist = Operations::mod(toLight);
                toLight = Operations::norm(toLight);
            }

            Ray shadowRay(shadowOrigin.toVector(), toLight.toVector());
            bool inShadow = false;

            for (auto& prim : primitives) {
                float shadowT;
                if (prim->intersect(shadowRay, shadowT) && shadowT > 0.001f && shadowT < dist) {
                    inShadow = true;
                    break;
                }
            }

            if (!inShadow) {
                Pixel contrib = {0,0,0};
                if (lights[l].width > 0 && lights[l].height > 0) {
                    // Luz de área: usamos calcularLuzAreaSample pasando el color del material
                    contrib = lights[l].calcularLuzAreaSample(origen, normal, material.toPixel(), lightSample);
                } else {
                    // Luz puntual: podemos usar la evaluación BSDF que maneja ks/kt
                    contrib = lights[l].calcularLuzBSDF(origen, normal, material, toLight, wo);
                }
                colorDirecto.r += contrib.r;
                colorDirecto.g += contrib.g;
                colorDirecto.b += contrib.b;
            }
        }
    }

    // ------------------ Muestreo para rebote indirecto ------------------
    // Decide tipo de rebote según el material
    Direction nextDir;
    Color weight = material.kd; // por defecto difuso
    bool isDelta = false;

    if (material.isSpecular()) {
        // Reflexión perfecta
        Direction incident = Operations::mult(wo, -1.0f);
        nextDir = Operations::reflect(incident, normal);
        isDelta = true;
        weight = material.ks;
    } else if (material.isDielectric()) {
    Direction incident = Operations::mult(wo, -1.0f);
    float cosThetaI = Operations::dot(incident, normal);

    float etaI = 1.0f, etaT = material.ior;
    Direction n = normal;

    if (cosThetaI > 0.0f) {
        n = Operations::mult(normal, -1.0f);
        std::swap(etaI, etaT);
        cosThetaI = Operations::dot(incident, n);
    } else {
        cosThetaI = -cosThetaI;
    }

    float F = Operations::fresnelDielectric(cosThetaI, etaI, etaT);
    float etaRatio = etaI / etaT;
    Vector3 refracted;
    bool canRefract = Operations::refract(incident.toVector(), n.toVector(), etaRatio, refracted);

    float xi = static_cast<float>(rand()) / RAND_MAX;
    if (!canRefract || xi < F) {
        // REFLECTION
        nextDir = Operations::reflect(incident, n);
        weight = material.ks; // neutral color
    } else {
        // TRANSMISSION
        nextDir = refracted;
        float etaCorr = 1.0f / (etaRatio * etaRatio);
        weight = Color(material.kt.R * etaCorr,
                       material.kt.G * etaCorr,
                       material.kt.B * etaCorr);
    }

    isDelta = true;

    float eps = 0.001f;
    float ndot = Operations::dot(nextDir, n);
    Point nextOrigin = (ndot > 0.0f)
        ? Operations::add(origen, Operations::mult(n, eps))
        : Operations::add(origen, Operations::mult(n, -eps));
    } else {
        // Difuso: muestreo hemisférico
        nextDir = Operations::randomHemisphereDirection(normal);
        isDelta = false;
        weight = material.kd;
    }

    // Traza el rayo de rebote
    Point nextOrigin = Operations::add(origen, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    float t = -1;
    float bestT = -1;
    Primitive* closest = nullptr;

    for (auto& prim : primitives) {
        if (prim->intersect(nextRay, t) && (t < bestT || bestT < 0) && t > 0) {
            bestT = t;
            closest = prim;
        }
    }

    Pixel indirect = {0,0,0};
    if (closest) {
        Point hit = Operations::add(nextRay.origin_.toPoint(), Operations::mult(nextRay.direction_.toDirection(), bestT));
        Direction hitNormal = closest->getPointNormal(hit);
        Material hitMaterial = closest->getMaterial();
        // Recursión: la nueva wo es opuesto a la dirección del rayo que llega al hit
        Direction newWo = Operations::mult(nextRay.direction_.toDirection(), -1.0f);
        Pixel rec = calcularReboteIndirecto(hit, hitNormal, hitMaterial, newWo, primitives, lights, depth + 1, maxBounces);

    // Modula por el peso del material (kd/ks/kt) por componente (sin dividir por pdf)
    indirect.r = rec.r * weight.R;
    indirect.g = rec.g * weight.G;
    indirect.b = rec.b * weight.B;
        // Reducir la magnitud de los rebotes difusos para evitar bleeding excesivo
        if (!isDelta) {
            indirect.r *= 0.2f;
            indirect.g *= 0.2f;
            indirect.b *= 0.2f;
        }
    }

    Pixel total;
    total.r = colorDirecto.r + indirect.r;
    total.g = colorDirecto.g + indirect.g;
    total.b = colorDirecto.b + indirect.b;

    return total;
}



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
            Pixel accumulatedColor = {0, 0, 0};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(i, j, true);
                float t = -1;
                float bestT = -1;
                Primitive* closestPrimitive = nullptr;
                Pixel color = {0, 0, 0};
                Pixel colorPrimitive = {0, 0, 0};

                for (size_t k = 0; k < primitives.size(); ++k) {
                    if (primitives[k]->intersect(ray, t)) {
                        if ((t < bestT || bestT == -1) && t > 0) {
                            bestT = t;
                            colorPrimitive = primitives[k]->getMaterial().toPixel();
                            closestPrimitive = primitives[k];
                        }
                    }
                }

                if (bestT > 0 && closestPrimitive != nullptr) {
                    Point hitPoint = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), bestT));
                    Direction normal = closestPrimitive->getPointNormal(hitPoint);
                    
                    // Si la superficie es emisiva, agregar su emisión
                    if (closestPrimitive->emitsLight()) {
                        Plane* emissivePlane = static_cast<Plane*>(closestPrimitive);
                        Light areaLight = emissivePlane->getAreaLight();
                        Pixel emision = areaLight.calcularRadianciaRectangular();
                        color.r += emision.r;
                        color.g += emision.g;
                        color.b += emision.b;
                    }
                    
                    std::vector<bool> inShadow(lights.size(), false);

                    for (size_t l = 0; l < lights.size(); ++l) {
                        Point shadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f));
                        Point lightSample;
                        Direction shadowRayDir;
                        float maxDist;

                        if (lights[l].width > 0 && lights[l].height > 0) {
                            lightSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                            shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                            maxDist = Operations::mod(shadowRayDir);
                            shadowRayDir = Operations::norm(shadowRayDir);
                        } else {
                            lightSample = lights[l].origen;
                            shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                            maxDist = Operations::mod(shadowRayDir);
                            shadowRayDir = Operations::norm(shadowRayDir);
                        }

                        Ray shadowRay(shadowOrigin.toVector(), shadowRayDir.toVector());

                        for (size_t k = 0; k < primitives.size(); ++k) {
                            float shadowT;
                            if (primitives[k]->intersect(shadowRay, shadowT) && shadowT > 0.001 && shadowT < maxDist) {
                                inShadow[l] = true;
                                break;
                            }
                        }

                        if (!inShadow[l]) {
                            Pixel lightContribution;
                            if (lights[l].width > 0 && lights[l].height > 0) {
                                lightContribution = lights[l].calcularLuzAreaSample(hitPoint, normal, colorPrimitive, lightSample);
                            } else {
                                lightContribution = lights[l].calcularLuz(hitPoint, normal, colorPrimitive);
                            }
                            color.r += lightContribution.r;
                            color.g += lightContribution.g;
                            color.b += lightContribution.b;
                        }
                    }

                    // Preparar material y dirección saliente (wo) para el muestreo de rebote
                    Material hitMat = closestPrimitive->getMaterial();
                    Direction wo = Operations::mult(ray.direction_.toDirection(), -1.0f);
                    Pixel indirect = calcularReboteIndirecto(hitPoint, normal, hitMat, wo, primitives, lights, 0, maxBounces);
                    color.r += indirect.r;
                    color.g += indirect.g;
                    color.b += indirect.b;
                }

                accumulatedColor.r += color.r;
                accumulatedColor.g += color.g;
                accumulatedColor.b += color.b;
            }

            accumulatedColor.r /= numRays;
            accumulatedColor.g /= numRays;
            accumulatedColor.b /= numRays;

            image.setPixel(i, j, accumulatedColor);
        }
    }

    return image;
}



