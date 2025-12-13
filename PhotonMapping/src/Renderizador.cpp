#include <cstdlib>
#include <cmath>
#include "Renderizador.hpp"
#include "PhotonMapper.hpp"
#include "Primitive.hpp"
#include "Plane.hpp"
#include "Operations.hpp"

// ------------------------------------------------------------
// AUXILIARES
// ------------------------------------------------------------

float randomOffset(float pixelSize) {
    double r = static_cast<double>(rand()) / RAND_MAX;
    return pixelSize * (r * 2.0 - 1.0);
}

bool hayLuzPuntual(const std::vector<Light>& lights) {
    for (const auto& luz : lights) {
        if (luz.width == 0.0f && luz.height == 0.0f) {
            return true;
        }
    }
    return false;
}

Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives) {
    float mejorT = -1.0f;
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
    if (colores.empty()) return {0, 0, 0};

    Pixel total = {0, 0, 0};
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

// ------------------------------------------------------------
// INTERSECCIÓN ADAPTADA (CORREGIDA)
// ------------------------------------------------------------

InterseccionData calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives) {
    InterseccionData inter;
    inter.hayInterseccion = false;
    inter.t = -1.0f;
    inter.primitiva = nullptr;

    float mejorT = 1e30f;

    for (auto& prim : primitives) {
        float t = -1.0f;
        if (prim->intersect(ray, t) && t > 0.001f) {
            if (t < mejorT) {
                mejorT = t;
                inter.hayInterseccion = true;
                inter.t = t;
                inter.primitiva = prim;
            }
        }
    }

    if (inter.hayInterseccion && inter.primitiva) {
        inter.punto = Operations::add(
            ray.origin_.toPoint(),
            Operations::mult(ray.direction_.toDirection(), inter.t)
        );
        inter.normal = inter.primitiva->getPointNormal(inter.punto);

        if (Operations::dot(inter.normal, ray.direction_.toDirection()) > 0) {
            inter.normal = Operations::mult(inter.normal, -1.0f);
        }
    }

    return inter;
}

// ------------------------------------------------------------
// SOMBRAS
// ------------------------------------------------------------

bool estaEnSombra(const Point& punto, const Direction& normal, const Light& luz, const std::vector<Primitive*>& primitives) {
    Point shadowOrigin = Operations::add(punto, Operations::mult(normal, 0.001f));
    Point lightSample = luz.origen;

    Direction shadowDir = Operations::sub(lightSample, shadowOrigin);
    float maxDist = Operations::mod(shadowDir);
    shadowDir = Operations::norm(shadowDir);

    Ray shadowRay(shadowOrigin.toVector(), shadowDir.toVector());

    for (auto& prim : primitives) {
        float t;
        if (prim->intersect(shadowRay, t) && t > 0.001f && t < maxDist) {
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------
// MUESTREO DE DIRECCIÓN
// ------------------------------------------------------------

void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
) {
    Direction incident = Operations::mult(wo, -1.0f);
    isDelta = false;
    weight = {0, 0, 0};

    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        weight = material.ks;
        isDelta = true;
        return;
    }

    if (material.isDielectric()) {
        Direction n = normal;
        bool entering = (Operations::dot(incident, n) < 0.0f);
        if (!entering) n = Operations::mult(n, -1.0f);

        float etaI = entering ? 1.0f : material.ior;
        float etaT = entering ? material.ior : 1.0f;

        Vector3 refracted;
        bool canRefract = Operations::refract(incident.toVector(), n.toVector(), etaI, etaT, refracted);

        if (!canRefract) {
            nextDir = Operations::reflect(incident, n);
            weight = material.ks;
            isDelta = true;
            return;
        }

        float Er = std::max({material.ks.R, material.ks.G, material.ks.B});
        float Et = std::max({material.kt.R, material.kt.G, material.kt.B});
        float sumE = Er + Et;
        if (sumE <= 0.0f) sumE = 1.0f;

        float Pr = Er / sumE;
        float Pt = Et / sumE;

        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pr) {
            nextDir = Operations::reflect(incident, n);
            weight = Color(material.ks.R / Pr, material.ks.G / Pr, material.ks.B / Pr);
        } else {
            nextDir = refracted;
            float etaRatio = etaI / etaT;
            float etaCorr = etaRatio * etaRatio;
            weight = Color(
                material.kt.R * etaCorr / Pt,
                material.kt.G * etaCorr / Pt,
                material.kt.B * etaCorr / Pt
            );
        }

        isDelta = true;
        return;
    }

    if (material.isPlastic()) {
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B});
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B});
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;

        float Pd = Ed / sum;
        float Ps = Es / sum;

        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Ps) {
            nextDir = Operations::reflect(incident, normal);
            weight = Color(material.ks.R / Ps, material.ks.G / Ps, material.ks.B / Ps);
            isDelta = true;
        } else {
            nextDir = Operations::randomHemisphereDirection(normal);
            weight = Color(material.kd.R / Pd, material.kd.G / Pd, material.kd.B / Pd);
            isDelta = false;
        }
        return;
    }

    nextDir = Operations::randomHemisphereDirection(normal);
    weight = material.kd;
    isDelta = false;
}

// ------------------------------------------------------------
// ILUMINACIÓN DIRECTA
// ------------------------------------------------------------

Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
) {
    Pixel color = {0, 0, 0};

    if (material.isEspejoPuro() || material.isDielectric()) {
        return color;
    }

    bool existeLuzPuntual = hayLuzPuntual(lights);

    if (existeLuzPuntual) {
        for (const auto& luz : lights) {
            Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
            Point lightSample = luz.origen;

            Direction toLight = Operations::sub(lightSample, shadowOrigin);
            float dist = Operations::mod(toLight);
            toLight = Operations::norm(toLight);

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
                Pixel contrib = luz.calcularLuzBSDF(origen, normal, material, toLight, wo);
                color.r += contrib.r * material.kd.R;
                color.g += contrib.g * material.kd.G;
                color.b += contrib.b * material.kd.B;
            }
        }
    }

    return color;
}

// ------------------------------------------------------------
// REBOTE INDIRECTO (DELTA)
// ------------------------------------------------------------

Pixel calcularReboteIndirectoConFotones(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    PhotonMapper* photonMapper,
    float pm_radius,
    int pm_nPhotons,
    KERNEL kernelType,
    int depth,
    int maxBounces
);

// ------------------------------------------------------------
// ILUMINACIÓN GLOBAL CON PHOTON MAPPING
// ------------------------------------------------------------

Pixel calcularIluminacionGlobalConFotones(
    const InterseccionData& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives,
    const Direction& wo,
    PhotonMapper* photonMapper,
    float pm_radius,
    int pm_nPhotons,
    KERNEL kernelType,
    int depth,
    int maxBounces
) {
    Pixel colorTotal = {0, 0, 0};
    const Material& material = inter.primitiva->getMaterial();

    if (inter.primitiva->emitsLight()) {
        const Plane* planePtr = static_cast<const Plane*>(inter.primitiva);
        const Light& light = planePtr->getAreaLight();
        Pixel emision = light.calcularRadianciaRectangular();
        return emision;
    }

    if (!material.isEspejoPuro() && !material.isDielectric()) {
        Pixel directa = calcularIluminacionDirecta(
            inter.punto, inter.normal, material, wo, primitives, luces
        );

        colorTotal.r += directa.r;
        colorTotal.g += directa.g;
        colorTotal.b += directa.b;
    }

    if (depth < maxBounces) {
        if (!material.isEspejoPuro() && !material.isDielectric() && photonMapper) {
            Color indirecta_pm = photonMapper->estimateRadiance(
                inter.punto, inter.normal, wo, material,
                pm_radius, pm_nPhotons, kernelType
            );

            // Debug: print first few indirect radiance samples for non-delta hits
            {
                static int pm_debug_samples = 0;
                if (pm_debug_samples < 10) {
                    pm_debug_samples++;
                    fprintf(stderr,
                        "PM Indirect #%d at (%.3f, %.3f, %.3f) n=(%.3f, %.3f, %.3f) radius=%.3f N=%d -> (R=%.6f, G=%.6f, B=%.6f)\n",
                        pm_debug_samples,
                        inter.punto.x, inter.punto.y, inter.punto.z,
                        inter.normal.x, inter.normal.y, inter.normal.z,
                        pm_radius, pm_nPhotons,
                        indirecta_pm.R, indirecta_pm.G, indirecta_pm.B
                    );
                }
            }

            colorTotal.r += indirecta_pm.R;
            colorTotal.g += indirecta_pm.G;
            colorTotal.b += indirecta_pm.B;
        }
        else if (material.isEspejoPuro() || material.isDielectric()) {
            Pixel indirecta = calcularReboteIndirectoConFotones(
                inter.punto, inter.normal, material, wo,
                primitives, luces, photonMapper,
                pm_radius, pm_nPhotons, kernelType,
                depth, maxBounces
            );

            colorTotal.r += indirecta.r;
            colorTotal.g += indirecta.g;
            colorTotal.b += indirecta.b;
        }
    }

    return colorTotal;
}

// ------------------------------------------------------------
// RECUSIÓN DELTA
// ------------------------------------------------------------

Pixel calcularReboteIndirectoConFotones(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    PhotonMapper* photonMapper,
    float pm_radius,
    int pm_nPhotons,
    KERNEL kernelType,
    int depth,
    int maxBounces
) {
    if (depth >= maxBounces) return {0, 0, 0};

    Direction nextDir;
    Color weight;
    bool isDelta = false;

    muestrearDireccionRebote(material, normal, wo, nextDir, weight, isDelta);

    if (!isDelta) return {0, 0, 0};

    float maxWeight = std::max({weight.R, weight.G, weight.B, 0.05f});

    if (depth > 2) {
        float xi = float(rand()) / RAND_MAX;
        if (xi > maxWeight) return {0, 0, 0};

        weight.R /= maxWeight;
        weight.G /= maxWeight;
        weight.B /= maxWeight;
    }

    Point nextOrigin = Operations::add(origen, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    InterseccionData inter = calcularInterseccionMasCercana(nextRay, primitives);
    Pixel indirecto = {0, 0, 0};

    if (inter.hayInterseccion && inter.primitiva) {
        if (inter.primitiva->emitsLight()) {
            const Plane* planePtr = static_cast<const Plane*>(inter.primitiva);
            const Light& light = planePtr->getAreaLight();
            indirecto = light.calcularRadianciaRectangular();
        }
        else {
            Direction newWo = Operations::mult(nextRay.direction_.toDirection(), -1.0f);

            Pixel rec = calcularIluminacionGlobalConFotones(
                inter, lights, primitives, newWo,
                photonMapper, pm_radius, pm_nPhotons, kernelType,
                depth + 1, maxBounces
            );

            indirecto.r = rec.r * weight.R;
            indirecto.g = rec.g * weight.G;
            indirecto.b = rec.b * weight.B;
        }
    }

    return indirecto;
}
