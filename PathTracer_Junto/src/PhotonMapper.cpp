#define _USE_MATH_DEFINES
#include "PhotonMapper.hpp"
#include "Operations.hpp"
#include "Renderizador.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

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

    //Si nPhotons es 0 no hacemos nada
    if (nPhotons <= 0) return;
   
    photons_.clear();

    const auto& lights = scene_->getLights();
    
    // --- 1. Identificar luces puntuales y calcular Flujo Total (Phi_total) ---
    Color totalScenePower = {0, 0, 0};
    std::vector<int> pointLightIndices;

    
    if (hayLuzPuntual(lights)){
        for (size_t i = 0; i < lights.size(); ++i) {
            // Asumiendo que las luces puntuales tienen width/height = 0.0f
            if (lights[i].width == 0.0f && lights[i].height == 0.0f) { 
                // Asumiendo que la Potencia/Flujo total está en light.power
                totalScenePower.R += lights[i].power.r; 
                totalScenePower.G += lights[i].power.g;
                totalScenePower.B += lights[i].power.b;
                pointLightIndices.push_back(i);
            }
        }
    }else{
        std::cerr << "No hay luces puntuales" << std::endl;
        return;
    }

    // --- 2. Calcular Flujo por Fotón (Phi_p) ---
    // Cada fotón lleva una fracción del flujo total de la escena.
    Color fluxPerPhoton(
        totalScenePower.R / nPhotons,
        totalScenePower.G / nPhotons,
        totalScenePower.B / nPhotons
    );
    
    // --- 3. Bucle de emisión y Random Walk ---
    for (int i = 0; i < nPhotons; ++i) {
        // Seleccionar luz aleatoria (solo de las puntuales)
        int lightIdx = pointLightIndices[rand() % pointLightIndices.size()];
        const Light& light = lights[lightIdx];

        Direction dir = Operations::sampleUniformSphereDirection();

        // Crear rayo desde la luz
        Ray photonRay(light.origen.toVector(), dir.toVector());

        // Iniciar el rastreo recursivo del fotón
        tracePhoton(photonRay, fluxPerPhoton, 0); 
    }
}
    


//--------------------------------------------------------------------
// FUNCION DE RASTREO DEL FOTÓN (RANDOM WALK)
//--------------------------------------------------------------------

// Función para rastrear el camino de un único fotón
void PhotonMapper::tracePhoton(Ray& ray, Color currentFlux, int depth) {
    
    // ----------------------------------------------------
    // Ruleta Rusa de Absorción
    // ----------------------------------------------------
    // Detener si la profundidad es excesiva
    if (depth >= MAX_PHOTON_DEPTH) return; 

    float p_continue = std::max({currentFlux.R, currentFlux.G, currentFlux.B});
    if (depth > 0) { // Aplicar RR solo después del primer rebote (o en difusos)
        if ((float)rand() / (float)RAND_MAX > p_continue) return;
        // Compensación del flujo por Ruleta Rusa
        currentFlux.R /= p_continue;
        currentFlux.G /= p_continue;
        currentFlux.B /= p_continue;
    }

    // ----------------------------------------------------
    // Intersección
    // ----------------------------------------------------
    // Reutilizas la función de tu Path Tracer
    Interseccion inter = calcularInterseccionMasCercana(ray, scene_->getPrimitives()); 
    
    // Parada: No hay intersección o golpea una luz (que ya fue contada en el flujo inicial)
    if (!inter.hayInterseccion || inter.primitiva->emitsLight()) return; 

    const Material& material = inter.primitiva->getMaterial();

    // ----------------------------------------------------
    // Almacenamiento del Fotón (Guardrail de Jensen)
    // ----------------------------------------------------
    
    // 1. NO almacenar en superficies Delta (espejo/dieléctrico)
    // 2. NO almacenar el primer rebote (depth == 0) si usas NEE en la fase de render.
    if (!material.isEspejoPuro() && !material.isDielectric()) {
        
        // Si usamos Next-Event Estimation (NEE) para la Luz Directa en el render
        if (depth > 0) { 
            Photon p;
            p.position = inter.punto.toVector(); 
            // La dirección incidente es el reverso de la dirección del rayo
            p.directionIncidente = Operations::mult(ray.direction_.toDirection(), -1.0f).toVector(); 
            p.flux = currentFlux.toPixel(); // Guardar el flujo como Pixel
            photons_.push_back(p);
        }
    }

    // ----------------------------------------------------
    // Muestreo del Rebote (Scatter)
    // ----------------------------------------------------
    Direction nextDir;
    Color throughput; // El factor por el que se multiplica el flujo (ej: k_d o k_s)
    
    // Necesitas implementar una versión de muestreo que devuelva el THROWPUT físico (k_d o k_s), NO el peso de MC.
    samplePhotonDirection(material, inter.normal, ray.direction_.toDirection(), nextDir, throughput);

    // Actualizar flujo por el factor de atenuación
    currentFlux.R *= throughput.R;
    currentFlux.G *= throughput.G;
    currentFlux.B *= throughput.B;

    // Si el flujo restante es insignificante, parar
    if (std::max({currentFlux.R, currentFlux.G, currentFlux.B}) < MIN_FLUX) return;

    // Generar el rayo siguiente
    Point nextOrigin = Operations::add(inter.punto, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    tracePhoton(nextRay, currentFlux, depth + 1);
}

// Muestrear la dirección del fotón tras el rebote

void PhotonMapper::samplePhotonDirection(
    const Material& material,
    const Direction& normal,
    const Direction& incidentDir, // Dirección saliente del fotón (wi, hacia el punto)
    Direction& nextDir,
    Color& throughput             // <-- FACTOR DE ATENUACIÓN FÍSICA (k_d o k_s)
) {
    throughput = {0, 0, 0}; 
    // incident: Dirección del rayo que golpea la superficie (wi)
    Direction incident = Operations::mult(incidentDir, -1.0f); 

    // ---------------- 1. Espejo puro (Delta) ----------------
    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        throughput = material.ks; 
        // El fotón atenúa su flujo por el coeficiente de reflexión.
        return;
    }

    // ---------------- 2. Dieléctrico (Transmisión/Reflexión Delta) ----------------
    if (material.isDielectric()) {
        Direction n = normal;
        // Determinar si el rayo entra o sale del medio (inversión de la normal si es necesario)
        bool entering = (Operations::dot(incident, n) < 0.0f);
        if (!entering) n = Operations::mult(n, -1.0f);

        float etaI = entering ? 1.0f : material.ior;
        float etaT = entering ? material.ior : 1.0f;

        Vector3 refracted;
        // Intenta refracción (necesario para detectar TIR)
        bool canRefract = Operations::refract(incident.toVector(), n.toVector(), etaI, etaT, refracted);

        if (!canRefract) {
            // --- REFLEXIÓN INTERNA TOTAL (TIR) ---
            nextDir = Operations::reflect(incident, n);
            throughput = material.ks; // El 100% del flujo especular se refleja
            return;
        }

        // Usamos la energía (k_s, k_t) para la probabilidad de elegir camino (Ruleta Rusa)
        float Er = std::max({material.ks.R, material.ks.G, material.ks.B});
        float Et = std::max({material.kt.R, material.kt.G, material.kt.B});
        float sumE = Er + Et;

        if (sumE <= 0.0f) sumE = 1.0f; 

        float Pr = Er / sumE; // Probabilidad de Reflejar
        
        //--------------Ruleta Rusa (para elegir camino)---------------
        float xi = static_cast<float>(rand()) / (float)RAND_MAX;

        if (xi < Pr) {
            // --- REFLEXIÓN ---
            nextDir = Operations::reflect(incident, n);
            throughput = material.ks; 
            // La atenuación es solo k_s. NO se divide por Pr.
        } else {
            // --- REFRACCIÓN ---
            nextDir = refracted;
            float etaRatio = etaI / etaT;
            // Corrección por cambio de fase (etaI/etaT)^2
            float etaCorr = etaRatio * etaRatio; 
            
            throughput = Color(
                material.kt.R * etaCorr,
                material.kt.G * etaCorr,
                material.kt.B * etaCorr
            );
            // La atenuación es k_t * etaCorr. NO se divide por Pt.
        }
        return;
    }
    
    // ---------------- 3. Plástico (Difuso + Especular NO-Delta) ----------------
    else if (material.isPlastic()) {
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B});
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B});
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;

        float Pd = Ed / sum; // Probabilidad de ser Difuso (para la RR)
        
        //-------------Ruleta Rusa (para elegir componente)--------------------------
        float xi = static_cast<float>(rand()) / (float)RAND_MAX;
        
        if (xi < Pd) {
            // Difuso: rebote al azar en el hemisferio
            nextDir = Operations::randomHemisphereDirection(normal);
            throughput = material.kd; 
            // Throughput = k_d. NO hay división por Pd.
        } else {
            // Especular: rebote reflejado (Aproximación para materiales rugosos)
            Direction incident_for_reflect = Operations::mult(incidentDir, -1.0f); 
            nextDir = Operations::reflect(incident_for_reflect, normal);
            throughput = material.ks; 
            // Throughput = k_s. NO hay división por Ps.
        }
        return;
    }
    
    // ---------------- 4. Difuso Puro ----------------
    else {
        nextDir = Operations::randomHemisphereDirection(normal);
        throughput = material.kd; // throughput = k_d
    }
}

//--------------------------------------------------------------------
// FUNCION DE ESTIMACIÓN DE RADIANCIA INDIRECTA EN UN PUNTO
//--------------------------------------------------------------------

Color PhotonMapper::estimateRadiance(
    const Point& p,
    const Direction& normal,
    const Direction& wo, // Dirección saliente (hacia la cámara)
    const Material& material, // <-- ¡Usamos este material!
    float maxRadius,
    int nPhotonsToFind
) const {
    Color L_indirect = {0, 0, 0};

    // ----------------------------------------------------
    // 0. Comprobación del Material
    // ----------------------------------------------------
    // Solo estimamos la luz indirecta para componentes NO-Delta (difusas o plásticas)
    if (material.isEspejoPuro() || material.isDielectric()) {
        return L_indirect;
    }

    // ----------------------------------------------------
    // 1. Búsqueda de Fotones y Cálculo del Radio (R)
    // ----------------------------------------------------
    
    Vector3 searchPos = p.toVector(); 
    
    // Llamada al KD-Tree: Obtiene punteros a los fotones más cercanos
    // foundPtrs: Lista de punteros a Photon* (o similar)
    auto foundPtrs = map_.nearest_neighbors(searchPos, nPhotonsToFind, maxRadius); 
    
    // Si no se encontraron fotones, salimos
    if (foundPtrs.empty()) {
        return L_indirect;
    }
    
    // Convertir punteros a copias y encontrar el radio máximo al mismo tiempo
    std::vector<Photon> foundPhotons;
    foundPhotons.reserve(foundPtrs.size());

    float actualSquaredRadius = 0.0f; // Inicializamos a 0
    
    for (const auto& photonPtr : foundPtrs) {
        // Asumiendo que foundPtrs contiene punteros (Photon*)
        const Photon& photon = *photonPtr;
        foundPhotons.push_back(photon);

        // Calcular distancia cuadrada: |p - photon.position|^2
        Vector3 diff = Operations::sub(p, Point(photon.position)).toVector();
        float distSq = Operations::dot(diff.toDirection(), diff.toDirection()); 
        
        // Actualizar el radio cuadrado real (el más grande encontrado)
        if (distSq > actualSquaredRadius) {
            actualSquaredRadius = distSq;
        }
    }
    
    // Si no se encontraron suficientes fotones, el radio real es el radio de la esfera
    // que contiene al fotón más lejano.
    float finalRadius = std::sqrt(actualSquaredRadius); 
    
    // El área del disco de estimación.
    float area = M_PI * finalRadius * finalRadius;

    // ----------------------------------------------------
    // 2. Estimación de Radiancia (Kernel Estimator)
    // ----------------------------------------------------
    
    // f_r (BSDF) para la estimación: SOLO COMPONENTE DIFUSA (k_d / π)

    Color kd_over_pi(
        material.kd.R / float(M_PI),
        material.kd.G / float(M_PI),
        material.kd.B / float(M_PI)
    );

    for (const auto& photon : foundPhotons) {
        
        Direction wi = photon.directionIncidente.toDirection();
        
        // Coseno del ángulo incidente (dot(wi, n))
        // Descartar fotones que vengan del lado equivocado
        float cos_theta = Operations::dot(wi, normal);
        if (cos_theta <= 1e-6f) continue;

        // L_i = f_r * Φ_p
        // (Multiplicamos la BSDF difusa (k_d/π) por el flujo del fotón)
        Color photon_contribution = Color(
            kd_over_pi.R * photon.flux.r,
            kd_over_pi.G * photon.flux.g,
            kd_over_pi.B * photon.flux.b
        );

        L_indirect.R += photon_contribution.R;
        L_indirect.G += photon_contribution.G;
        L_indirect.B += photon_contribution.B;
    }

    // 3. Normalizar por el área
    // Radiancia = Sum(f_r * Φ_p) / Area
    L_indirect.R /= area;
    L_indirect.G /= area;
    L_indirect.B /= area;

    return L_indirect;
}