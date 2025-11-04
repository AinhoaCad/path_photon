#include <cstdlib> // Para rand()
#include "Renderizador.hpp"

// Función auxiliar: devuelve un desplazamiento aleatorio dentro de un pixel
float randomOffset(float pixelSize) {
    // rand() / RAND_MAX genera un número en [0,1]
    // Multiplicamos por 2 y restamos 1 → [-1,1]
    // Luego multiplicamos por el tamaño del píxel para obtener el offset real
    double r = (double)rand() / RAND_MAX;  
    return pixelSize * (r * 2.0 - 1.0);    
}

// Devuelve el color del material de la primitiva más cercana intersectada por el rayo.
// Si no hay intersección, devuelve negro.
Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives) {
    float mejorT = -1.0f;
    //por defecto si no pilla primitiva es negro
    Pixel color = {0, 0, 0};

    for (auto& primitive : primitives) {
        float t = -1.0f;
        if (primitive->intersect(ray, t)) {
            if ((t < mejorT || mejorT < 0) && t > 0) {
                mejorT = t;
                color = primitive->getMaterial().toPixel();
            }
        }
    }
    return color;
}


Pixel promedioColores(const std::vector<Pixel>& colores) {
    if (colores.empty()) return {0,0,0};

    Pixel total = {0,0,0};
    for (const auto& c : colores) {
        total.r += c.r;
        total.g += c.g;
        total.b += c.b;
    }

    float n = static_cast<float>(colores.size());
    total.r /= n;
    total.g /= n;
    total.b /= n;
    return total;
}

// Devuelve la intersección más cercana del rayo con las primitivas
Interseccion calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives) {
    Interseccion inter;
    inter.hayInterseccion = false;
    inter.t = -1;
    inter.primitiva = nullptr;

    for (auto& prim : primitives) {
        float t = -1;
        if (prim->intersect(ray, t) && t > 0.001f && (t < inter.t || inter.t < 0)) {
            inter.hayInterseccion = true;
            inter.t = t;
            inter.primitiva = prim;
        }
    }

    if (inter.hayInterseccion && inter.primitiva) {
        inter.punto = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), inter.t));
        inter.normal = inter.primitiva->getPointNormal(inter.punto);
    }

    return inter;
}

// Comprueba si un punto está en sombra respecto a una luz
bool estaEnSombra(const Point& punto, const Direction& normal, const Light& luz, const std::vector<Primitive*>& primitives) {
    Point shadowOrigin = Operations::add(punto, Operations::mult(normal, 0.001f)); // Evita autocontacto
    Point lightSample;
    Direction shadowDir;
    float maxDist;

    // Luz de área
    if (luz.width > 0 && luz.height > 0) {
        lightSample = luz.samplePointOnArea((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    } else {
        lightSample = luz.origen;
    }

    shadowDir = Operations::sub(lightSample, shadowOrigin);
    maxDist = Operations::mod(shadowDir);
    shadowDir = Operations::norm(shadowDir);

    Ray shadowRay(shadowOrigin.toVector(), shadowDir.toVector());

    for (auto& prim : primitives) {
        float t;
        if (prim->intersect(shadowRay, t) && t > 0.001f && t < maxDist) {
            return true; // Algo bloquea la luz
        }
    }
    return false;
}

Pixel calcularIluminacion(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives
) {
    Pixel colorFinal = {0, 0, 0};
    const Material& material = inter.primitiva->getMaterial();
    Pixel colorMat = material.toPixel();

    // --- 1️⃣ Si el objeto emite luz, sumarla directamente ---
    if (inter.primitiva->emitsLight()) {
        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
        Light areaLight = emisiva->getAreaLight();
        Pixel emision = areaLight.calcularRadianciaRectangular();
        colorFinal.r += emision.r;
        colorFinal.g += emision.g;
        colorFinal.b += emision.b;
    }

    // --- 2️⃣ Recorremos todas las luces ---
    for (const auto& luz : luces) {
        // Comprobamos si el punto está en sombra respecto a esta luz
        if (estaEnSombra(inter.punto, inter.normal, luz, primitives))
            continue;

        Pixel contrib = {0, 0, 0};

        // --- 3️⃣ Luz de área ---
        if (luz.width > 0 && luz.height > 0) {
            // Promediamos varias muestras para reducir ruido
            int numSamples = 8;
            for (int s = 0; s < numSamples; ++s) {
                float u = static_cast<float>(rand()) / RAND_MAX;
                float v = static_cast<float>(rand()) / RAND_MAX;
                Point sample = luz.samplePointOnArea(u, v);

                Pixel sampleContrib = luz.calcularLuzAreaSample(
                    inter.punto, inter.normal, colorMat, sample
                );
                contrib.r += sampleContrib.r;
                contrib.g += sampleContrib.g;
                contrib.b += sampleContrib.b;
            }
            contrib.r /= numSamples;
            contrib.g /= numSamples;
            contrib.b /= numSamples;
        }
        // --- 4️⃣ Luz puntual ---
        else {
            contrib = luz.calcularLuz(inter.punto, inter.normal, colorMat);
        }

        // --- 5️⃣ Sumar contribución final ---
        colorFinal.r += contrib.r;
        colorFinal.g += contrib.g;
        colorFinal.b += contrib.b;
    }

    return colorFinal;
}

Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
) {
    Pixel color = {0, 0, 0};
    bool isDeltaMaterial = material.isSpecular() || material.isDielectric();
    if (isDeltaMaterial) return color;

    for (const auto& luz : lights) {
        Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
        Point lightSample;
        Direction toLight;
        float dist;

        if (luz.width > 0 && luz.height > 0) {
            lightSample = luz.samplePointOnArea((float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
            toLight = Operations::sub(lightSample, shadowOrigin);
            dist = Operations::mod(toLight);
            toLight = Operations::norm(toLight);
        } else {
            lightSample = luz.origen;
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
            Pixel contrib = (luz.width > 0 && luz.height > 0)
                ? luz.calcularLuzAreaSample(origen, normal, material.toPixel(), lightSample)
                : luz.calcularLuzBSDF(origen, normal, material, toLight, wo);

            color.r += contrib.r;
            color.g += contrib.g;
            color.b += contrib.b;
        }
    }

    return color;
}

void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
) {
    if (material.isSpecular()) {
        Direction incident = Operations::mult(wo, -1.0f);
        nextDir = Operations::reflect(incident, normal);
        weight = material.ks;
        isDelta = true;
        return;
    }

    if (material.isDielectric()) {
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
            nextDir = Operations::reflect(incident, n);
            weight = material.ks;
        } else {
            nextDir = refracted;
            float etaCorr = 1.0f / (etaRatio * etaRatio);
            weight = Color(material.kt.R * etaCorr,
                           material.kt.G * etaCorr,
                           material.kt.B * etaCorr);
        }

        isDelta = true;
        return;
    }

    // Difuso
    nextDir = Operations::randomHemisphereDirection(normal);
    weight = material.kd;
    isDelta = false;
}

