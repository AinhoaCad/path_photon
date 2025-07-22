#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Light.hpp"
#include <ctime> // Para usar srand
#include <cstdlib> // Para generar números aleatorios


Camera::Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp)
    : origen(origen), left(l), up(u), forward(f), pixelWidth(anchp), pixelHeight(altp) {
    // Calcular dimensiones físicas del plano de la imagen
    onepixelWidth = anchoPlano / pixelWidth;
    onepixelHeight = altoPlano / pixelHeight;
}

// Método para generar un rayo hacia el centro de un píxel dado (i, j)
Ray Camera::generateRay(int i, int j, std::string n) {
    float pixelCenterX, pixelCenterY;

    if(n == "uno"){
        // Calculamos las coordenadas del centro del píxel (i, j)
        pixelCenterX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2;
        pixelCenterY = - (j + 0.5f) * onepixelHeight + altoPlano / 2;
    }
    else if (n == "varios"){
        double aleatorioX = (double)rand() / RAND_MAX;   //0.0 y 1.0
        aleatorioX = aleatorioX * 2 - 1; // -1.0 y 1.0
        aleatorioX = onepixelWidth * aleatorioX;
            
        double aleatorioY = (double)rand() / RAND_MAX;   //0.0 y 1.0
        aleatorioY = aleatorioY * 2 - 1; // -1.0 y 1.0
        aleatorioY = onepixelWidth * aleatorioY;

        // Calculamos las coordenadas del centro del píxel (i, j)
        pixelCenterX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2 + aleatorioX;
        pixelCenterY = - (j + 0.5f) * onepixelHeight + altoPlano / 2 + aleatorioY;
    }
    // El punto en el plano de la imagen en coordenadas de la cámara
    Direction rayDirection = Direction(pixelCenterX, pixelCenterY, 1);
    Operations::norm(rayDirection); // Aseguramos que el vector esté normalizado
    
    std::vector<Matrix> matrices = {left.getHomMatrix(), up.getHomMatrix(), forward.getHomMatrix(), origen.getHomMatrix()};
    Matrix m(matrices); // Creamos la matriz de cambio de base al sistema global

    // Multiplicamos la dirección del rayo por la matriz de cambio de base
    Matrix a = m * rayDirection.getHomMatrix();

    rayDirection = Direction(a); // Convertimos el resultado en un vector dirección
    return Ray(origen.toVector(), rayDirection.toVector()); // Devolvemos el rayo con origen en la cámara y dirección calculada
}

Image Camera::generateImage(const std::vector<Primitive*>& primitives) {
    Image image(pixelWidth, pixelHeight);
    for (int i = 0; i < pixelHeight; i++) {
        for (int j = 0; j < pixelWidth; j++) {
            Ray ray = generateRay(i, j, "uno");
            float t = -1;
            float mejorT = -1;
            Pixel p = {0, 0, 0}; // Si no intersecta, el color del píxel será este
            for (auto& primitive : primitives) {
                if (primitive->intersect(ray, t)) {
                    if ((t < mejorT || mejorT == -1) && t > 0) {
                        mejorT = t;
                        p = primitive->getMaterial().toPixel(); // Obtenemos el color del material de la primitiva
                    }
                }
            }
            image.setPixel(i, j, p);
        }
    }
    return image;
}

Image Camera::generateImageMultipleRays(const std::vector<Primitive*>& primitives, int numRays) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(NULL)));

    int numPrimitives = static_cast<int>(primitives.size());
    std::vector<float> t(numRays, -1);
    std::vector<float> mejorT(numRays, -1);
    std::vector<int> mejorPrimitiva(numRays, -1);

    for (int i = 0; i < pixelHeight; i++) {
        for (int j = 0; j < pixelWidth; j++) {

            for (int r = 0; r < numRays; r++) {
                Ray ray = generateRay(i, j, "varios");
                t[r] = -1;
                mejorT[r] = -1;

                for (int k = 0; k < numPrimitives; k++) {
                    if (primitives[k]->intersect(ray, t[r])) {
                        if ((t[r] < mejorT[r] || mejorT[r] == -1) && t[r] > 0) {
                            mejorT[r] = t[r];
                            mejorPrimitiva[r] = k;
                        }
                    }
                }
            }

            Pixel p = {0, 0, 0}; // Color por defecto si no hay intersección

            for (int r = 0; r < numRays; r++) {
                if (mejorT[r] != -1) {
                    Pixel p2 = primitives[mejorPrimitiva[r]]->getMaterial().toPixel(); // Obtenemos el color del material de la primitiva
                    p.r += p2.r;
                    p.g += p2.g;
                    p.b += p2.b;
                }
            }

            // Promediamos el color
            p.r /= numRays;
            p.g /= numRays;
            p.b /= numRays;

            image.setPixel(i, j, p);
        }
    }

    return image;
}

Image Camera::generateImageWithLightingOneRay(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights) {
    Image image(pixelWidth, pixelHeight);

    int numLights = static_cast<int>(lights.size());

    for (int i = 0; i < pixelHeight; ++i) {
        for (int j = 0; j < pixelWidth; ++j) {
            Ray ray = generateRay(i, j, "uno");
            float t = -1;
            float mejorT = -1;
            Pixel p = {0, 0, 0}; // Color negro por defecto

            // Comprobar intersecciones con objetos
            Primitive* closestPrimitive = nullptr;
            for (int k = 0; k < static_cast<int>(primitives.size()); k++) {
                if (primitives[k]->intersect(ray, t)) {
                    if ((t < mejorT || mejorT == -1) && t > 0) {
                        mejorT = t;
                        p = primitives[k]->getMaterial().toPixel(); // Color del objeto en la intersección
                        closestPrimitive = primitives[k];
                    }
                }
            }

            // Solo calculamos la luz si hay una intersección
            if (mejorT > 0 && closestPrimitive != nullptr) {
                Point hitPoint = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), mejorT));
                Direction normal = closestPrimitive->getPointNormal(hitPoint);

                std::vector<bool> inShadow(numLights, false); // Vector para estados de sombra

                // Calcula contribución de cada fuente de luz
                for (int l = 0; l < numLights; l++) {
                    Point shadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f)); 

                    Direction shadowRayDir = Operations::sub(lights[l].origen, shadowOrigin);
                    float maxDist = Operations::mod(shadowRayDir);
                    shadowRayDir = Operations::norm(shadowRayDir);
                    Ray shadowRay(shadowOrigin.toVector(), shadowRayDir.toVector());

                    // Comprobamos si hay intersección en la dirección de la luz
                    for (int k = 0; k < static_cast<int>(primitives.size()); k++) {
                        float shadowT;
                        if (primitives[k]->intersect(shadowRay, shadowT) && shadowT > 0.001f && shadowT < maxDist) {
                            inShadow[l] = true;
                            break;
                        }
                    }
                }

                int numSombras = 0;
                for (int l = 0; l < numLights; l++) {
                    if (!inShadow[l]) {
                        Pixel iluminacion = lights[l].calcularLuz(hitPoint, normal, p);
                        p.r = std::min(p.r + iluminacion.r, 255.0f);
                        p.g = std::min(p.g + iluminacion.g, 255.0f);
                        p.b = std::min(p.b + iluminacion.b, 255.0f);
                    } else {
                        numSombras++;
                    }
                }

                if (numSombras == numLights) {
                    p = {0, 0, 0}; // Todo en sombra -> negro
                }
            }
            image.setPixel(i, j, p);
        }
    }
    return image;
}



Image Camera::generateImageWithLightingMultipleRays(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights, int numRays){
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < pixelHeight; ++i) {
        for (int j = 0; j < pixelWidth; ++j) {
            Pixel accumulatedColor{0.f, 0.f, 0.f};

            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(i, j, "varios");
                float bestT = -1.f;
                Primitive* closestPrimitive = nullptr;
                Pixel color{0.f, 0.f, 0.f};

                // Buscamos la intersección más cercana
                for (auto* primitive : primitives) {
                    float t = -1.f;
                    if (primitive->intersect(ray, t) && t > 0.f && (bestT < 0.f || t < bestT)) {
                        bestT = t;
                        closestPrimitive = primitive;
                        color = primitive->getMaterial().toPixel(); // Obtenemos el color del material
                    }
                }

                if (bestT > 0.f && closestPrimitive != nullptr) {
                    Point hitPoint = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), bestT));
                    Direction normal = closestPrimitive->getPointNormal(hitPoint);

                    // Vector para sombras
                    std::vector<bool> inShadow(lights.size(), false);

                    for (size_t l = 0; l < lights.size(); ++l) {
                        Point shadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f));
                        Direction shadowRayDir = Operations::sub(lights[l].origen, shadowOrigin);
                        float maxDist = Operations::mod(shadowRayDir);
                        shadowRayDir = Operations::norm(shadowRayDir);

                        Ray shadowRay(shadowOrigin.toVector(), shadowRayDir.toVector());

                        for (auto* primitive : primitives) {
                            float shadowT;
                            if (primitive->intersect(shadowRay, shadowT)) {
                                if (shadowT > 0.001f && shadowT < maxDist) {
                                    inShadow[l] = true;
                                    break;
                                }
                            }
                        }
                    }

                    // Suma contribuciones de luz no en sombra
                    for (size_t l = 0; l < lights.size(); ++l) {
                        if (!inShadow[l]) {
                            Pixel lightContribution = lights[l].calcularLuz(hitPoint, normal, color);
                            color.r += lightContribution.r;
                            color.g += lightContribution.g;
                            color.b += lightContribution.b;
                        }
                    }
                }

                accumulatedColor.r += color.r;
                accumulatedColor.g += color.g;
                accumulatedColor.b += color.b;
            }

            // Promediar color
            accumulatedColor.r /= numRays;
            accumulatedColor.g /= numRays;
            accumulatedColor.b /= numRays;

            image.setPixel(i, j, accumulatedColor);
        }
    }

    return image;
}
