#define _USE_MATH_DEFINES
#include "PhotonMapper.hpp"
#include "Operations.hpp"
#include "Renderizador.hpp"
#include "Primitive.hpp"
#include "Scene.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include "kdtree.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_PHOTON_DEPTH 10
#define MIN_FLUX 1e-9 // Umbral bajo para descarte por energía

// --------------------------------------------------------------------
// CALCULAR INTERSECCIÓN MÁS CERCANA
// --------------------------------------------------------------------
InterseccionData PhotonMapper::calcularInterseccionMasCercana_PM(
    Ray& ray, const std::vector<Primitive*>& primitives) const
{
    InterseccionData inter;
    inter.t = -1.0f;
    inter.hayInterseccion = false;
    inter.primitiva = nullptr;
    float t_candidato = 1e30f;

    for (const auto& primitive : primitives) {
        float t_actual;
        if (primitive->intersect(ray, t_actual) && t_actual > 0.001f) {
            if (t_actual < t_candidato) {
                t_candidato = t_actual;
                inter.hayInterseccion = true;
                inter.primitiva = primitive;
                inter.t = t_actual;
            }
        }
    }

    if (inter.hayInterseccion) {
        Vector3 intersectionVec =
            Operations::add(
                ray.origin().toPoint(),
                Operations::mult(ray.direction().toDirection(), inter.t)
            ).toVector(); 
        inter.punto = intersectionVec.toPoint();
        inter.normal = inter.primitiva->getPointNormal(inter.punto);

        if (Operations::dot(inter.normal, ray.direction().toDirection()) > 0) {
            inter.normal = Operations::mult(inter.normal, -1.0f);
        }
    }

    return inter;
}

// --------------------------------------------------------------------
// Constructor y Destructor
// --------------------------------------------------------------------
PhotonMapper::PhotonMapper(Scene* scene)
    : scene_(scene), map_(nullptr)
{
    srand(static_cast<unsigned>(time(nullptr)));
}

PhotonMapper::~PhotonMapper() {
    if (map_) {
        delete map_;
        map_ = nullptr;
    }
}

// --------------------------------------------------------------------
// Construir KD-Tree
// --------------------------------------------------------------------
void PhotonMapper::buildPhotonMap() {
    if (photons_.empty()) {
        std::cerr << "Advertencia: No se almacenó ningún fotón. El mapa estará vacío." << std::endl;
        return;
    }

    if (map_) delete map_;
    map_ = new PhotonMap(photons_, PhotonAxisPosition());
}

// --------------------------------------------------------------------
// Emitir fotones
// --------------------------------------------------------------------
void PhotonMapper::emitPhotons(int nPhotons, float amplificationFactor) {
    if (nPhotons <= 0) return;

    photons_.clear();
    const auto& lights = scene_->getLights();
    Color totalScenePower = {0, 0, 0};
    std::vector<int> pointLightIndices;

    extern bool hayLuzPuntual(const std::vector<Light>&);

    if (hayLuzPuntual(lights)) {
        for (size_t i = 0; i < lights.size(); ++i) {
            if (lights[i].width == 0.0f && lights[i].height == 0.0f) {
                totalScenePower.R += lights[i].power.r;
                totalScenePower.G += lights[i].power.g;
                totalScenePower.B += lights[i].power.b;
                pointLightIndices.push_back(i);
            }
        }
    } else {
        std::cerr << "No hay luces puntuales activas para emitir fotones." << std::endl;
        return;
    }

    if (pointLightIndices.empty()) return;

    // Amplify per-photon flux to make indirect visible (tuning factor)
    const float M = amplificationFactor;
    std::cout << "Emitiendo " << nPhotons << " fotones (M=" << M << ")..." << std::endl;
    Color fluxPerPhoton(
        (totalScenePower.R / nPhotons) * M,
        (totalScenePower.G / nPhotons) * M,
        (totalScenePower.B / nPhotons) * M
    );

    for (int i = 0; i < nPhotons; ++i) {
        int lightIdx = pointLightIndices[rand() % pointLightIndices.size()];
        const Light& light = lights[lightIdx];
        Direction dir = Operations::sampleUniformSphereDirection();
        Ray photonRay(light.origen.toVector(), dir.toVector());
        tracePhoton(photonRay, fluxPerPhoton, 0);
    }
}

// --------------------------------------------------------------------
// Trazado de fotón
// --------------------------------------------------------------------
void PhotonMapper::tracePhoton(Ray& ray, Color currentFlux, int depth) {
    if (depth >= MAX_PHOTON_DEPTH) return;

    if (std::max({currentFlux.R, currentFlux.G, currentFlux.B}) < MIN_FLUX) return;

    InterseccionData inter = calcularInterseccionMasCercana_PM(ray, scene_->getPrimitives());
    if (!inter.hayInterseccion || inter.primitiva->emitsLight()) return;

    const Material& material = inter.primitiva->getMaterial();

    Direction nextDir;
    Color throughput;

    samplePhotonDirection(material, inter.normal, ray.direction_.toDirection(), nextDir, throughput);

    if (depth > 0) { 
        float p_continue = std::max({throughput.R, throughput.G, throughput.B}); 
        if (p_continue <= 1e-6f) return;
        if ((float)rand() / RAND_MAX > p_continue) return;
        currentFlux.R /= p_continue;
        currentFlux.G /= p_continue;
        currentFlux.B /= p_continue;
    }

    if (!material.isEspejoPuro() && !material.isDielectric()) {
        if (depth > 0) {
            Photon p;
            p.position = inter.punto.toVector();
            p.directionIncidente = ray.direction_;
            p.flux = currentFlux.toPixel();
            photons_.push_back(p);
        }
    }

    currentFlux.R *= throughput.R;
    currentFlux.G *= throughput.G;
    currentFlux.B *= throughput.B;

    Point nextOrigin = Operations::add(inter.punto, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());
    tracePhoton(nextRay, currentFlux, depth + 1);
}

// --------------------------------------------------------------------
// Estimación de radiancia
// --------------------------------------------------------------------
Color PhotonMapper::estimateRadiance(
    const Point& p,
    const Direction& normal,
    const Direction& wo,
    const Material& material,
    float maxRadius,
    int nPhotonsToFind,
    KERNEL kernelType
) const {
    Color L_indirect = {0, 0, 0};
    Vector3 searchPos = p.toVector();

    if (!map_) return L_indirect;

    auto foundPtrs = map_->nearest_neighbors(searchPos, nPhotonsToFind, maxRadius);
    if (foundPtrs.empty()) return L_indirect;

    float finalRadiusSq = 0.0f;
    Color totalFlux = {0, 0, 0};
    Color kd_BRDF(material.kd.R, material.kd.G, material.kd.B); 

    for (const auto& photonPtr : foundPtrs) {
        const Photon& photon = *photonPtr;
        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection());
        if (distSq > finalRadiusSq) finalRadiusSq = distSq;
    }

    for (const auto& photonPtr : foundPtrs) {
        const Photon& photon = *photonPtr;
        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection());
        float dist = std::sqrt(distSq);
        Direction wi = photon.directionIncidente.toDirection();
        float cos_theta = Operations::dot(wi, normal);
        if (cos_theta <= 1e-6f) continue;

        float weight = 1.0f;
        float radius = std::sqrt(finalRadiusSq);

        switch (kernelType) {
            case CONE: {
                float k = 2.0f;
                float wpc = 1.0f - dist / (k * radius);
                weight = std::max(0.0f, wpc);
                break;
            }
            case GAUSSIAN: {
                float alpha = 0.918f;
                float beta = 1.953f;
                float R_sq = radius * radius;
                weight = alpha * (1.0f - ((1.0f - expf(-beta * (distSq / (2.0f * R_sq)))) / (1.0f - expf(-beta))));
                break;
            }
            case BOX:
            default:
                weight = 1.0f;
                break;
        }

        Color photon_contribution(
            kd_BRDF.R * photon.flux.r * cos_theta * weight,
            kd_BRDF.G * photon.flux.g * cos_theta * weight,
            kd_BRDF.B * photon.flux.b * cos_theta * weight
        );

        totalFlux.R += photon_contribution.R;
        totalFlux.G += photon_contribution.G;
        totalFlux.B += photon_contribution.B;
    }

    float finalRadius = std::sqrt(finalRadiusSq);
    // Debug: print found photon count and final radius for first few queries
    {
        static int pm_query_debug = 0;
        if (pm_query_debug < 10) {
            pm_query_debug++;
            fprintf(stderr,
                "PM Query #%d at (%.3f, %.3f, %.3f) n=(%.3f, %.3f, %.3f) -> found=%zu finalRadius=%.3f (maxR=%.3f, N=%d)\n",
                pm_query_debug,
                p.x, p.y, p.z,
                normal.x, normal.y, normal.z,
                foundPtrs.size(), finalRadius, maxRadius, nPhotonsToFind);
        }
    }
    float area = M_PI * finalRadius * finalRadius;
    float normalization_factor = 1.0f / (area * float(M_PI)); 

    if (kernelType == CONE) {
        float k = 2.0f;
        normalization_factor = 1.0f / (area * (1.0f - (2.0f / (3.0f * k))) * float(M_PI));
    }

    L_indirect.R = totalFlux.R * normalization_factor;
    L_indirect.G = totalFlux.G * normalization_factor;
    L_indirect.B = totalFlux.B * normalization_factor;

    return L_indirect;
}
// --------------------------------------------------------------------
// Muestreo de dirección del fotón
// --------------------------------------------------------------------
void PhotonMapper::samplePhotonDirection(
    const Material& material,
    const Direction& normal,
    const Direction& incidentDir,
    Direction& nextDir,
    Color& throughput
) {
    throughput = {0, 0, 0};
    Direction incident = Operations::mult(incidentDir, -1.0f);

    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        throughput = material.ks;
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
            throughput = material.ks;
            return;
        }

        float Er = std::max({material.ks.R, material.ks.G, material.ks.B});
        float Et = std::max({material.kt.R, material.kt.G, material.kt.B});
        float sumE = Er + Et;
        if (sumE <= 0.0f) sumE = 1.0f;
        float Pr = Er / sumE;
        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pr) {
            nextDir = Operations::reflect(incident, n);
            throughput = Color(material.ks.R / Pr, material.ks.G / Pr, material.ks.B / Pr);
        } else {
            nextDir = refracted;
            float Pt = 1.0f - Pr;
            if (Pt < 1e-6f) Pt = 1e-6f;
            float etaRatio = etaI / etaT;
            float etaCorr = etaRatio * etaRatio;
            throughput = Color((material.kt.R * etaCorr) / Pt, (material.kt.G * etaCorr) / Pt, (material.kt.B * etaCorr) / Pt);
        }
        return;
    }

    if (material.isPlastic()) {
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B});
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B});
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;
        float Pd = Ed / sum;
        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pd) {
            nextDir = Operations::randomHemisphereDirection(normal);
            throughput = material.kd;
        } else {
            nextDir = Operations::reflect(incident, normal);
            throughput = material.ks;
        }
        return;
    }

    nextDir = Operations::randomHemisphereDirection(normal);
    throughput = material.kd;
}
