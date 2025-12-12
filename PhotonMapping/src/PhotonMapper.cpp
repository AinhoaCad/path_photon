#define _USE_MATH_DEFINES
#include "PhotonMapper.hpp"
#include "Operations.hpp"
#include "Renderizador.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm> // Necesario para std::max

#define MAX_PHOTON_DEPTH 10
#define MIN_FLUX 1e-4

// Constructor
PhotonMapper::PhotonMapper(Scene* scene)
    : scene_(scene) {
    srand(static_cast<unsigned>(time(nullptr))); // Semilla aleatoria
}

// Construir el KD-Tree con los fotones
PhotonMap PhotonMapper::buildPhotonMap() {
    map_ = PhotonMap(photons_, PhotonAxisPosition());
    return map_;
}

//--------------------------------------------------------------------
// FUNCION DE EMITIR FOTONES DESDE LAS LUCES PUNTUALES
//--------------------------------------------------------------------

// Emitir nPhotons desde luces puntuales
void PhotonMapper::emitPhotons(int nPhotons) {

    if (nPhotons <= 0) return;

    photons_.clear();

    const auto& lights = scene_->getLights();
    
    // --- 1. Identificar luces puntuales y calcular Flujo Total ---
    Color totalScenePower = {0, 0, 0};
    std::vector<int> pointLightIndices;

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
        std::cerr << "No hay luces puntuales" << std::endl;
        return;
    }

    // --- 2. Flujo por fotón ---
    Color fluxPerPhoton(
        totalScenePower.R / nPhotons,
        totalScenePower.G / nPhotons,
        totalScenePower.B / nPhotons
    );

    // --- 3. Emitir fotones ---
    for (int i = 0; i < nPhotons; ++i) {
        int lightIdx = pointLightIndices[rand() % pointLightIndices.size()];
        const Light& light = lights[lightIdx];

        Direction dir = Operations::sampleUniformSphereDirection();

        Ray photonRay(light.origen.toVector(), dir.toVector());

        tracePhoton(photonRay, fluxPerPhoton, 0);
    }
}

//--------------------------------------------------------------------
// RASTREO DE FOTÓN (RANDOM WALK)
//--------------------------------------------------------------------

void PhotonMapper::tracePhoton(Ray& ray, Color currentFlux, int depth) {

    if (depth >= MAX_PHOTON_DEPTH) return;

    float p_continue = std::max({currentFlux.R, currentFlux.G, currentFlux.B});

    if (depth > 0) {
        if ((float)rand() / RAND_MAX > p_continue) return;

        if (p_continue > 1e-6f) {
            currentFlux.R /= p_continue;
            currentFlux.G /= p_continue;
            currentFlux.B /= p_continue;
        }
    }

    Interseccion inter = calcularInterseccionMasCercana(ray, scene_->getPrimitives());
    if (!inter.hayInterseccion || inter.primitiva->emitsLight()) return;

    const Material& material = inter.primitiva->getMaterial();

    if (!material.isEspejoPuro() && !material.isDielectric()) {
        if (depth > 0) {
            Photon p;
            p.position = inter.punto.toVector();
            p.directionIncidente = ray.direction_;
            p.flux = currentFlux.toPixel();
            photons_.push_back(p);
        }
    }

    Direction nextDir;
    Color throughput;

    samplePhotonDirection(material, inter.normal, ray.direction_.toDirection(), nextDir, throughput);

    currentFlux.R *= throughput.R;
    currentFlux.G *= throughput.G;
    currentFlux.B *= throughput.B;

    if (std::max({currentFlux.R, currentFlux.G, currentFlux.B}) < MIN_FLUX) return;

    Point nextOrigin = Operations::add(inter.punto, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    tracePhoton(nextRay, currentFlux, depth + 1);
}

//--------------------------------------------------------------------
// ESTIMACIÓN DE RADIANCIA
//--------------------------------------------------------------------

Color PhotonMapper::estimateRadiance(
    const Point& p,
    const Direction& normal,
    const Direction& wo,
    const Material& material,
    float maxRadius,
    int nPhotonsToFind
) const {

    Color L_indirect = {0, 0, 0};

    Vector3 searchPos = p.toVector();
    auto foundPtrs = map_.nearest_neighbors(searchPos, nPhotonsToFind, maxRadius);

    if (foundPtrs.empty())
        return L_indirect;

    std::vector<Photon> foundPhotons;
    foundPhotons.reserve(foundPtrs.size());

    float actualSquaredRadius = 0.0f;

    for (const auto& photonPtr : foundPtrs) {
        const Photon& photon = *photonPtr;
        foundPhotons.push_back(photon);

        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection());

        if (distSq > actualSquaredRadius)
            actualSquaredRadius = distSq;
    }

    float finalRadius = std::sqrt(actualSquaredRadius);
    float area = M_PI * finalRadius * finalRadius;

    Color kd_over_pi(
        material.kd.R / float(M_PI),
        material.kd.G / float(M_PI),
        material.kd.B / float(M_PI)
    );

    for (const auto& photon : foundPhotons) {
        Direction wi = photon.directionIncidente.toDirection();

        float cos_theta = Operations::dot(wi, normal);
        if (cos_theta <= 1e-6f) continue;

        Color photon_contribution(
            kd_over_pi.R * photon.flux.r * cos_theta,
            kd_over_pi.G * photon.flux.g * cos_theta,
            kd_over_pi.B * photon.flux.b * cos_theta
        );

        L_indirect.R += photon_contribution.R;
        L_indirect.G += photon_contribution.G;
        L_indirect.B += photon_contribution.B;
    }

    L_indirect.R /= area;
    L_indirect.G /= area;
    L_indirect.B /= area;

    return L_indirect;
}

//--------------------------------------------------------------------
// MUESTREO DE DIRECCIÓN DEL FOTÓN
//--------------------------------------------------------------------

void PhotonMapper::samplePhotonDirection(
    const Material& material,
    const Direction& normal,
    const Direction& incidentDir,
    Direction& nextDir,
    Color& throughput
) {
    throughput = {0, 0, 0};
    Direction incident = Operations::mult(incidentDir, -1.0f);

    // ---- Espejo puro ----
    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        throughput = material.ks;
        return;
    }

    // ---- Dieléctrico ----
    if (material.isDielectric()) {

        Direction n = normal;
        bool entering = (Operations::dot(incident, n) < 0.0f);
        if (!entering) n = Operations::mult(n, -1.0f);

        float etaI = entering ? 1.0f : material.ior;
        float etaT = entering ? material.ior : 1.0f;

        Vector3 refracted;
        bool canRefract = Operations::refract(
            incident.toVector(), n.toVector(), etaI, etaT, refracted
        );

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
        float Pt = 1.0f - Pr;
        if (Pt < 1e-6f) Pt = 1e-6f;

        float xi = float(rand()) / RAND_MAX;

        if (xi < Pr) {
            nextDir = Operations::reflect(incident, n);
            throughput = Color(
                material.ks.R / Pr,
                material.ks.G / Pr,
                material.ks.B / Pr
            );
        } else {
            nextDir = refracted;
            float etaRatio = etaI / etaT;
            float etaCorr = etaRatio * etaRatio;
            throughput = Color(
                (material.kt.R * etaCorr) / Pt,
                (material.kt.G * etaCorr) / Pt,
                (material.kt.B * etaCorr) / Pt
            );
        }
        return;
    }

    // ---- Plástico ----
    if (material.isPlastic()) {
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B});
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B});
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;

        float Pd = Ed / sum;

        float xi = float(rand()) / RAND_MAX;

        if (xi < Pd) {
            nextDir = Operations::randomHemisphereDirection(normal);
            throughput = material.kd;
        } else {
            Direction incident_for_reflect = Operations::mult(incidentDir, -1.0f);
            nextDir = Operations::reflect(incident_for_reflect, normal);
            throughput = material.ks;
        }
        return;
    }

    // ---- Difuso puro ----
    nextDir = Operations::randomHemisphereDirection(normal);
    throughput = material.kd;
}