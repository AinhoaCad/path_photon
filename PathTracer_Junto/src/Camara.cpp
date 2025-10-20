#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Light.hpp"
#include "Plane.hpp"
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
    rayDirection = Operations::norm(rayDirection); // Aseguramos que el vector esté normalizado
    
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

            // Buscar la primitiva más cercana
            Primitive* closestPrimitive = nullptr;
            for (int k = 0; k < static_cast<int>(primitives.size()); k++) {
                if (primitives[k]->intersect(ray, t)) {
                    if ((t < mejorT || mejorT == -1) && t > 0) {
                        mejorT = t;
                        p = primitives[k]->getMaterial().toPixel();
                        closestPrimitive = primitives[k];
                    }
                }
            }

            // Si hay intersección, calcular iluminación
            if (mejorT > 0 && closestPrimitive != nullptr) {
                Point hitPoint = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), mejorT));
                Direction normal = closestPrimitive->getPointNormal(hitPoint);

                // Si la superficie es emisiva, agregar su emisión
                if (closestPrimitive->emitsLight()) {
                    Plane* emissivePlane = static_cast<Plane*>(closestPrimitive);
                    Light areaLight = emissivePlane->getAreaLight();
                    Pixel emision = areaLight.calcularRadianciaRectangular();
                    p.r += emision.r;
                    p.g += emision.g;
                    p.b += emision.b;
                }

                std::vector<bool> inShadow(numLights, false);

                for (int l = 0; l < numLights; l++) {
                    Point shadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f));
                    Point lightSample;
                    Direction shadowRayDir;
                    float maxDist;

                    // Diferenciar tipos de luz
                    if (lights[l].width > 0 && lights[l].height > 0) {
                        // Luz de área rectangular
                        lightSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                        shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                        maxDist = Operations::mod(shadowRayDir);
                        shadowRayDir = Operations::norm(shadowRayDir);
                    } else {
                        // Luz puntual (o por ahora cualquier otro tipo futuro)
                        lightSample = lights[l].origen;
                        shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                        maxDist = Operations::mod(shadowRayDir);
                        shadowRayDir = Operations::norm(shadowRayDir);
                    }

                    Ray shadowRay(shadowOrigin.toVector(), shadowRayDir.toVector());

                    // Verificar sombras
                    for (int k = 0; k < static_cast<int>(primitives.size()); k++) {
                        float shadowT;
                        if (primitives[k]->intersect(shadowRay, shadowT) && shadowT > 0.001f && shadowT < maxDist) {
                            inShadow[l] = true;
                            break;
                        }
                    }
                }

                // Calcular contribución de luz
                int numSombras = 0;
                for (int l = 0; l < numLights; l++) {
                    if (!inShadow[l]) {
                        Pixel iluminacion = {0, 0, 0};
                        if (lights[l].width > 0 && lights[l].height > 0) {
                            // Luz de área - tomar múltiples muestras
                            int numLightSamples = 16;
                            Pixel totalIlum = {0, 0, 0};
                            
                            for (int ls = 0; ls < numLightSamples; ls++) {
                                Point areaSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                                
                                // Verificar sombras para esta muestra específica
                                Point areaShadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f));
                                Direction areaShadowDir = Operations::sub(areaSample, areaShadowOrigin);
                                float areaDist = Operations::mod(areaShadowDir);
                                areaShadowDir = Operations::norm(areaShadowDir);
                                Ray areaShadowRay(areaShadowOrigin.toVector(), areaShadowDir.toVector());
                                
                                bool areaInShadow = false;
                                for (int k = 0; k < static_cast<int>(primitives.size()); k++) {
                                    float areaShadowT;
                                    if (primitives[k]->intersect(areaShadowRay, areaShadowT) && areaShadowT > 0.001f && areaShadowT < areaDist) {
                                        areaInShadow = true;
                                        break;
                                    }
                                }
                                
                                if (!areaInShadow) {
                                    Pixel sampleIlum = lights[l].calcularLuzAreaSample(hitPoint, normal, p, areaSample);
                                    totalIlum.r += sampleIlum.r;
                                    totalIlum.g += sampleIlum.g;
                                    totalIlum.b += sampleIlum.b;
                                }
                            }
                            
                            // Promediar las muestras
                            iluminacion.r = totalIlum.r / numLightSamples;
                            iluminacion.g = totalIlum.g / numLightSamples;
                            iluminacion.b = totalIlum.b / numLightSamples;
                        } else {
                            // Luz puntual
                            iluminacion = lights[l].calcularLuz(hitPoint, normal, p);
                        }
                        p.r = std::min(p.r + iluminacion.r, 255.0f);
                        p.g = std::min(p.g + iluminacion.g, 255.0f);
                        p.b = std::min(p.b + iluminacion.b, 255.0f);
                    } else {
                        numSombras++;
                    }
                }

                // Todo en sombra
                if (numSombras == numLights) {
                    p = {0, 0, 0};
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
            Pixel accumulatedColor = {0, 0, 0}; // Acumular el color de cada rayo
            
            for (int r = 0; r < numRays; ++r) {
                Ray ray = generateRay(i, j, "varios");
                float t = -1;
                float bestT = -1;
                Primitive* closestPrimitive = nullptr;
                Pixel color = {0, 0, 0}; // Color negro por defecto para cada rayo
                Pixel colorPrimitive = {0,0,0};

                // Intersección con primitivas
                for (size_t k = 0; k < primitives.size(); k++) {
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

                    // Calcular sombras y contribución de cada luz
                    for (size_t l = 0; l < lights.size(); ++l) {
                        Point shadowOrigin = Operations::add(hitPoint, Operations::mult(normal, 0.001f));
                        Point lightSample;
                        Direction shadowRayDir;
                        float maxDist;

                        // Diferenciar tipos de luz
                        if (lights[l].width > 0 && lights[l].height > 0) {
                            // Luz de área
                            lightSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                            shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                            maxDist = Operations::mod(shadowRayDir);
                            shadowRayDir = Operations::norm(shadowRayDir);
                        } else {
                            // Luz puntual
                            lightSample = lights[l].origen;
                            shadowRayDir = Operations::sub(lightSample, shadowOrigin);
                            maxDist = Operations::mod(shadowRayDir);
                            shadowRayDir = Operations::norm(shadowRayDir);
                        }

                        Ray shadowRay(shadowOrigin.toVector(), shadowRayDir.toVector());

                        // Comprobar intersecciones con la escena para sombras
                        for (size_t k = 0; k < primitives.size(); k++) {
                            float shadowT;
                            if (primitives[k]->intersect(shadowRay, shadowT) && shadowT > 0.001f && shadowT < maxDist) {
                                inShadow[l] = true;
                                break;
                            }
                        }

                        // Añadir contribución de luz si no está en sombra
                        if (!inShadow[l]) {
                            Pixel lightContribution;
                            if (lights[l].width > 0 && lights[l].height > 0) {
                                // Luz de área - tomar múltiples muestras
                                int numLightSamples = 16; // Número de muestras de la luz de área
                                Pixel totalContribution = {0, 0, 0};
                                
                                for (int ls = 0; ls < numLightSamples; ls++) {
                                    Point areaSample = lights[l].samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
                                    
                                    // Verificar sombras para esta muestra específica
                                    Direction areaShadowDir = Operations::sub(areaSample, shadowOrigin);
                                    float areaDist = Operations::mod(areaShadowDir);
                                    areaShadowDir = Operations::norm(areaShadowDir);
                                    Ray areaShadowRay(shadowOrigin.toVector(), areaShadowDir.toVector());
                                    
                                    bool areaInShadow = false;
                                    for (size_t k = 0; k < primitives.size(); k++) {
                                        float areaShadowT;
                                        if (primitives[k]->intersect(areaShadowRay, areaShadowT) && areaShadowT > 0.001f && areaShadowT < areaDist) {
                                            areaInShadow = true;
                                            break;
                                        }
                                    }
                                    
                                    if (!areaInShadow) {
                                        Pixel sampleContrib = lights[l].calcularLuzAreaSample(hitPoint, normal, colorPrimitive, areaSample);
                                        totalContribution.r += sampleContrib.r;
                                        totalContribution.g += sampleContrib.g;
                                        totalContribution.b += sampleContrib.b;
                                    }
                                }
                                
                                // Promediar las muestras
                                lightContribution.r = totalContribution.r / numLightSamples;
                                lightContribution.g = totalContribution.g / numLightSamples;
                                lightContribution.b = totalContribution.b / numLightSamples;
                            } else {
                                // Luz puntual
                                lightContribution = lights[l].calcularLuz(hitPoint, normal, colorPrimitive);
                            }
                            color.r += lightContribution.r;
                            color.g += lightContribution.g;
                            color.b += lightContribution.b;
                        }
                    }
                }

                // Acumula el color de cada rayo
                accumulatedColor.r += color.r;
                accumulatedColor.g += color.g;
                accumulatedColor.b += color.b;
            }

            // Promediar los rayos
            accumulatedColor.r /= numRays;
            accumulatedColor.g /= numRays;
            accumulatedColor.b /= numRays;

            image.setPixel(i, j, accumulatedColor);
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
    Pixel colorDirecto = {0, 0, 0};
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
        // Intentar refractar, si hay TIR se refleja
        Direction incident = Operations::mult(wo, -1.0f);
        float cosTheta = Operations::dot(incident, normal);
        float eta = (cosTheta > 0.0f) ? (1.0f / material.ior) : material.ior;
        Direction refracted;
        bool ok = Operations::refract(incident, normal, eta, refracted);
        if (ok) {
            nextDir = refracted;
            isDelta = true;
            weight = material.kt;
        } else {
            // TIR -> reflexión
            nextDir = Operations::reflect(incident, normal);
            isDelta = true;
            weight = material.ks;
        }
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

        // Modula por el peso del material (kd/ks/kt) por componente
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
                Ray ray = generateRay(i, j, "varios");
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

        
