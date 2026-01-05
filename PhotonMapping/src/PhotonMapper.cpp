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
void PhotonMapper::emitPhotons(int nPhotons) {
    if (nPhotons <= 0) return;

    photons_.clear(); // photon_map = empty
    const auto& lights = scene_->getLights();
    Color totalScenePower = {0, 0, 0};
    std::vector<int> pointLightIndices;

    extern bool hayLuzPuntual(const std::vector<Light>&);

    // lights = scene.lights
    //Ignoraremos luces de area para la emision de fotones
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
    std::cout << "Emitiendo " << nPhotons << std::endl;

    // Calculo del flujo por foton (NOSOTROS LO HACEMOS COMO (TOTAL_POWER/N)*M) fisicamente solo seria TOTAL_POWER/N
    Color fluxPerPhoton(
        (totalScenePower.R / nPhotons) * 4 * M_PI,
        (totalScenePower.G / nPhotons) * 4 * M_PI,
        (totalScenePower.B / nPhotons) * 4 * M_PI
    );
    
    //sampleo de rayos
    for (int i = 0; i < nPhotons; ++i) {

        // choose light
        int lightIdx = pointLightIndices[rand() % pointLightIndices.size()];
        const Light& light = lights[lightIdx];

        // sample outgoing direction
        Direction dir = Operations::sampleUniformSphereDirection();
        Ray photonRay(light.origen.toVector(), dir.toVector());

        // trace photon
        tracePhoton(photonRay, fluxPerPhoton, 0);
    }
}

// --------------------------------------------------------------------
// Trazado de fotón
// --------------------------------------------------------------------
void PhotonMapper::tracePhoton(Ray& ray, Color currentFlux, int depth) {
    // Condiciones de parada

    if (depth >= MAX_PHOTON_DEPTH) return; // if depth > maxDepth: stop

    if (std::max({currentFlux.R, currentFlux.G, currentFlux.B}) < MIN_FLUX) return; // if flux too small: stop

    // hit = intersect(ray)
    InterseccionData inter = calcularInterseccionMasCercana_PM(ray, scene_->getPrimitives());

    // if no hit: stop
    // if hit light: stop
    if (!inter.hayInterseccion || inter.primitiva->emitsLight()) return;

    const Material& material = inter.primitiva->getMaterial();

    Direction nextDir;
    Color throughput;
    
    // wi -> sample wo from BRDF
    //throughput = fr * cos / pdf      throughput es el peso del rebote

    samplePhotonDirection(material, inter.normal, ray.direction_.toDirection(), nextDir, throughput);

    // Russian Roulette

    if (depth > 0) { 
        // if rand() > p: terminate
        float p_continue = std::max({throughput.R, throughput.G, throughput.B});
        // Clamp to [0,1] to be a valid probability
        if (p_continue < 0.0f) p_continue = 0.0f;
        if (p_continue > 1.0f) p_continue = 1.0f;
        if (p_continue <= 1e-6f) return;
        if ((float)rand() / RAND_MAX > p_continue) return;

        // flux /= p
        currentFlux.R /= p_continue;
        currentFlux.G /= p_continue;
        currentFlux.B /= p_continue;
    }

    // if not IS_DELTA: store photon
    if (!material.isEspejoPuro() && !material.isDielectric()) {
        if (depth > 0) {    //el primer rebote no se almacena
            Photon p;
            p.position = inter.punto.toVector();
            p.directionIncidente = ray.direction_;
            p.flux = currentFlux.toPixel();
            photons_.push_back(p);
        }
    }
    
    // flux *= throughput
    currentFlux.R *= throughput.R;
    currentFlux.G *= throughput.G;
    currentFlux.B *= throughput.B;
    
    // trace_photon(next_ray)
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


    // Resultado final: radiancia indirecta L_r(x, wo)
    Color L_indirect = {0, 0, 0};

    // Posición del punto de shading en formato vector
    Vector3 searchPos = p.toVector();

    if (!map_) return L_indirect; // Si no existe mapa de fotones, no hay iluminación indirecta

    // ------------------------------------------------------------
    // 1) BÚSQUEDA DE FOTONES CERCANOS (k-nearest neighbors)
    // ------------------------------------------------------------

    // Devuelve hasta k fotones dentro de maxRadius
    auto foundPtrs = map_->nearest_neighbors(searchPos, nPhotonsToFind, maxRadius);

    // Si no se encuentra ningún fotón, no hay contribución indirecta
    if (foundPtrs.empty()) return L_indirect;

    // ------------------------------------------------------------
    // 2) CÁLCULO DEL RADIO FINAL R (distancia al fotón más lejano)
    // ------------------------------------------------------------

    float finalRadiusSq = 0.0f;

    // Acumulador del numerador del estimador de densidad
    Color totalFlux = {0, 0, 0};

    // BRDF difusa (Lambert): f_r = kd / π
    Color kd_BRDF(material.kd.R / M_PI, material.kd.G / M_PI, material.kd.B / M_PI); 

    // Recorremos los fotones encontrados para calcular R^2
    for (const auto& photonPtr : foundPtrs) {
        const Photon& photon = *photonPtr;

        // Distancia al punto de shading
        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection());

        // Guardamos la mayor distancia (radio del disco de estimación)
        if (distSq > finalRadiusSq) finalRadiusSq = distSq;
    }

    // ------------------------------------------------------------
    // 3) SUMA DE CONTRIBUCIONES DE CADA FOTÓN
    // ------------------------------------------------------------

    for (const auto& photonPtr : foundPtrs) {
        const Photon& photon = *photonPtr;

        // Distancia del fotón al punto x
        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection());
        float dist = std::sqrt(distSq);

        // Dirección incidente del fotón (ω_i): el fotón viaja con dirección ω_p
        // Para el coseno de incidencia usamos n · (-ω_p)
        Direction wi = Operations::mult(photon.directionIncidente.toDirection(), -1.0f);

        // Término coseno: n · (-ω_p)
        float cos_theta = Operations::dot(wi, normal);  // contribución
        if (cos_theta <= 1e-6f) continue; // descarta fotones por detrás


        // --------------------------------------------------------
        // 4) KERNEL ESPACIAL K(r / R)
        // --------------------------------------------------------
        float weight = 1.0f;
        float radius = std::sqrt(finalRadiusSq);

        switch (kernelType) {
            // conico 
            case CONE: {
                float k = 2.0f;
                float wpc = 1.0f - dist / (k * radius);
                weight = std::max(0.0f, wpc);
                break;
            }
            // gaussiano
            case GAUSSIAN: {
                float alpha = 0.918f;
                float beta = 1.953f;
                float R_sq = radius * radius;
                weight = alpha * (1.0f - ((1.0f - expf(-beta * (distSq / (2.0f * R_sq)))) / (1.0f - expf(-beta))));
                break;
            }
            // caja
            case BOX:
            default:
                weight = 1.0f;
                break;
        }

        // --------------------------------------------------------
        // 5) CONTRIBUCIÓN DE UN FOTÓN
        // --------------------------------------------------------
        // Φ_p · f_r · cosθ · K(r / R)
        Color photon_contribution(
            kd_BRDF.R * photon.flux.r * cos_theta * weight,
            kd_BRDF.G * photon.flux.g * cos_theta * weight,
            kd_BRDF.B * photon.flux.b * cos_theta * weight
        );
        
        // Acumulamos el numerador del estimador
        totalFlux.R += photon_contribution.R;
        totalFlux.G += photon_contribution.G;
        totalFlux.B += photon_contribution.B;
    }

    // ------------------------------------------------------------
    // 6) NORMALIZACIÓN DEL ESTIMADOR DE DENSIDAD
    // ------------------------------------------------------------

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
    

    // Área del disco de estimación: πR²
    float area = M_PI * finalRadius * finalRadius;

    // Normalización básica:
    // Ecuación de photon mapping: L_r(x, wo) = Σ(Φ_p · f_r · cos(θ) · K(r/R)) / (π·R²)
    // La BRDF difusa ya incluye 1/π: f_r = kd/π
    // Por tanto: normalization_factor = 1 / (π·R²)

    float normalization_factor = 1.0f / area; 

    if (kernelType == CONE) {
        float k = 2.0f;
        normalization_factor = 1.0f / (area * (1.0f - (2.0f / (3.0f * k))));
    }

    // ---------------------------------------------------------------------
    // 7) RESULTADO FINAL: RADIANCIA INDIRECTA = Σ(Φ_p · f_r · cos(θ) · K(r/R)) / (π·R²)
    // ---------------------------------------------------------------------
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
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B}); // reflectancia difusa
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B}); // reflectancia especular
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;
        float Pd = Ed / sum;
        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pd) {
            // Difuso: muestreo hemisferio con coseno -> pdf = cos/π
            nextDir = Operations::randomHemisphereDirection(normal);
            // throughput = fr * cos / pdf / P(sel) -> (kd/π) * cos / (cos/π) / Pd = kd / Pd
            float denom = (Pd < 1e-6f) ? 1e-6f : Pd;
            throughput = Color(material.kd.R / denom, material.kd.G / denom, material.kd.B / denom);

            // Debug: verificar throughput = fr * cos / (pdf_total)
            {
                static int thr_dbg = 0;
                if (thr_dbg < 5) {
                    float cosTheta = std::max(0.0f, Operations::dot(Operations::norm(nextDir), Operations::norm(normal)));
                    float pdf_cond = cosTheta / float(M_PI);         // pdf condicional del hemisferio con coseno
                    float pdf_total = Pd * pdf_cond;                  // pdf total con selección de lóbulo
                    if (cosTheta > 1e-5f && pdf_total > 1e-6f) {
                        Color fr(material.kd.R / float(M_PI), material.kd.G / float(M_PI), material.kd.B / float(M_PI));
                        Color expected(
                            fr.R * cosTheta / pdf_total,
                            fr.G * cosTheta / pdf_total,
                            fr.B * cosTheta / pdf_total
                        );
                        fprintf(stderr,
                            "Plastic diffuse throughput: cos=%.3f pdf_total=%.5f expected=(%.3f, %.3f, %.3f) actual=(%.3f, %.3f, %.3f)\n",
                            cosTheta, pdf_total,
                            expected.R, expected.G, expected.B,
                            throughput.R, throughput.G, throughput.B
                        );
                        thr_dbg++;
                    }
                }
            }
        } else {
            // Especular: delta -> pdf = 1, throughput = ks / (1 - Pd)
            nextDir = Operations::reflect(incident, normal);
            float Ps = 1.0f - Pd;
            float denom = (Ps < 1e-6f) ? 1e-6f : Ps;
            throughput = Color(material.ks.R / denom, material.ks.G / denom, material.ks.B / denom);

            // Debug: verificar throughput especular con selección de lóbulo
            {
                static int thr_dbg_s = 0;
                if (thr_dbg_s < 3) {
                    float pdf_total = std::max(1e-6f, (1.0f - Pd));   // delta: pdf condicional = 1, pdf total = Ps
                    Color fr = material.ks;                            // BRDF especular ideal tratada como delta con energía ks
                    Color expected(fr.R / pdf_total, fr.G / pdf_total, fr.B / pdf_total);
                    fprintf(stderr,
                        "Plastic specular throughput: pdf_total=%.5f expected=(%.3f, %.3f, %.3f) actual=(%.3f, %.3f, %.3f)\n",
                        pdf_total,
                        expected.R, expected.G, expected.B,
                        throughput.R, throughput.G, throughput.B
                    );
                    thr_dbg_s++;
                }
            }
        }
        return;
    }

    // Debug: para material difuso puro (no plástico), comprobar fr*cos/pdf del hemisferio con coseno
    {
        static int thr_dbg_diffuse = 0;
        if (thr_dbg_diffuse < 3 && !material.isEspejoPuro() && !material.isDielectric() && !material.isPlastic()) {
            float cosTheta = std::max(0.0f, Operations::dot(Operations::norm(nextDir), Operations::norm(normal)));
            float pdf = cosTheta / float(M_PI);
            if (cosTheta > 1e-5f && pdf > 1e-6f) {
                Color fr(material.kd.R / float(M_PI), material.kd.G / float(M_PI), material.kd.B / float(M_PI));
                Color expected(
                    fr.R * cosTheta / pdf,
                    fr.G * cosTheta / pdf,
                    fr.B * cosTheta / pdf
                );
                fprintf(stderr,
                    "Diffuse throughput: cos=%.3f pdf=%.5f expected=(%.3f, %.3f, %.3f) actual=(%.3f, %.3f, %.3f)\n",
                    cosTheta, pdf,
                    expected.R, expected.G, expected.B,
                    throughput.R, throughput.G, throughput.B
                );
                thr_dbg_diffuse++;
            }
        }
    }

    nextDir = Operations::randomHemisphereDirection(normal);
    throughput = material.kd;
}
