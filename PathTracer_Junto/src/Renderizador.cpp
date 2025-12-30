#include <cstdlib> // Para rand()
#include "Renderizador.hpp"

//---------------------------------------------------------
//FUNCIONES AUXILIARES PARA RENDERIZADO
//---------------------------------------------------------

// Función auxiliar: devuelve un desplazamiento aleatorio dentro de un pixel
float randomOffset(float pixelSize) {
    // rand() / RAND_MAX genera un número en [0,1]
    // Multiplicamos por 2 y restamos 1 → [-1,1]
    // Luego multiplicamos por el tamaño del píxel para obtener el offset real
    double r = (double)rand() / RAND_MAX;  
    return pixelSize * (r * 2.0 - 1.0);    
}

bool hayLuzPuntual(const std::vector<Light>& lights) {
    for (const auto& luz : lights) {
        if (luz.width == 0.0f && luz.height == 0.0f) {
            return true; // Encontrada
        }
    }
    return false; // Ninguna luz puntual
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
    lightSample = luz.origen;
    
    /*
    if (luz.width > 0 && luz.height > 0) {
        lightSample = luz.samplePointOnArea((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    } else {
        lightSample = luz.origen;
    }
    */
    
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

//---------------------------------------------------------
//FUNCIONES PARA RENDERIZADO SIN REBOTE INDIRECTO
//---------------------------------------------------------

//---------------- Calcula iluminación para las imagenes que no tienen rebote indirecto ----------------
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
            /*
            //COMENTARIO TEMPORAL
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
            */
            
            //SIMPLIFICAMOS LO ANTERIOR
            Point origen = luz.origen;
            contrib = luz.calcularLuzAreaSample(
                    inter.punto, inter.normal, colorMat, origen
                );
            
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

//---------------------------------------------------------
//FUNCIONES PARA RENDERIZADO CON REBOTE INDIRECTO
//---------------------------------------------------------

// ---------------- Muestreo de nueva dirección según material ----------------
void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
) {
    Direction incident = Operations::mult(wo, -1.0f);

    // ---------------- Espejo puro ----------------
    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        weight = material.ks;
        isDelta = true;
        return;
    }

    // ---------------- Dieléctrico ----------------
    if (material.isDielectric()) {

        // La función refract ahora maneja internamente el ajuste de normal y eta
        // Solo necesitamos pasar la normal original y los índices base
        Direction n = normal;
        bool entering = (Operations::dot(incident, n) < 0.0f);
        
        float etaI = 1.0f;           // Índice del aire
        float etaT = material.ior;   // Índice del material

        Vector3 refracted;
        // Intenta refracción (necesario para detectar TIR)
        bool canRefract = Operations::refract(incident.toVector(), n.toVector(), etaI, etaT, refracted);

        if (!canRefract) {
            // --- REFLEXIÓN INTERNA TOTAL (TIR) ---
            Direction reflectNormal = entering ? n : Operations::mult(n, -1.0f);
            nextDir = Operations::reflect(incident, reflectNormal);
            weight = material.ks; // Usamos k_s como peso reflejado
            isDelta = true;
            return;
        }

        // Usamos la energía (k_s, k_t) para la probabilidad simplificada
        float Er = std::max({material.ks.R, material.ks.G, material.ks.B});
        float Et = std::max({material.kt.R, material.kt.G, material.kt.B});
        float sumE = Er + Et;

        if (sumE <= 0.0f) sumE = 1.0f; // evitar cero

        float Pr = Er / sumE; // Probabilidad de Reflejar
        float Pt = Et / sumE; // Probabilidad de Refractar

        //--------------Ruleta Rusa---------------
        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pr) {
            // --- REFLEXIÓN (comportamiento igual que espejo)---
            Direction reflectNormal = entering ? n : Operations::mult(n, -1.0f);
            nextDir = Operations::reflect(incident, reflectNormal);
            // Weight = k_s / P_r
            weight = Color(
                material.ks.R / std::max(Pr, 1e-6f),
                material.ks.G / std::max(Pr, 1e-6f),
                material.ks.B / std::max(Pr, 1e-6f)
            );
        } else {
            // --- REFRACCIÓN ---
            nextDir = refracted;
            // Determinar la dirección del cambio de medio
            float etaRatio = entering ? (etaI / etaT) : (etaT / etaI);
            // Corrección por cambio de fase (eta_i/eta_t)^2
            float etaCorr = etaRatio * etaRatio; 
            // Weight = (k_t * Corrección_eta) / P_t
            weight = Color(
                (material.kt.R * etaCorr) / std::max(Pt, 1e-6f),
                (material.kt.G * etaCorr) / std::max(Pt, 1e-6f),
                (material.kt.B * etaCorr) / std::max(Pt, 1e-6f)
            );
        }

        isDelta = true; 
        return;
    }

    // ---------------- Plástico (difuso + especular) ----------------
    if (material.isPlastic()) {
        float Ed = std::max({material.kd.R, material.kd.G, material.kd.B});
        float Es = std::max({material.ks.R, material.ks.G, material.ks.B});
        float sum = Ed + Es;
        if (sum <= 0.0f) sum = 1.0f;

        float Pd = Ed / sum;
        float Ps = Es / sum;

        //-------------Ruleta Rusa--------------------------
        float xi = static_cast<float>(rand()) / RAND_MAX;
        if (xi < Ps) {

            // Comportamiento espejo
            nextDir = Operations::reflect(incident, normal);
            weight = Color(material.ks.R / std::max(Ps, 0.05f),
                           material.ks.G / std::max(Ps, 0.05f),
                           material.ks.B / std::max(Ps, 0.05f));
            isDelta = true;
        } else {
            // Comportamiento difuso
            nextDir = Operations::randomHemisphereDirection(normal);
            weight = Color(material.kd.R / std::max(Pd, 0.05f),
                           material.kd.G / std::max(Pd, 0.05f),
                           material.kd.B / std::max(Pd, 0.05f));
            isDelta = false;
        }
        return;
    }

    // ---------------- Difuso puro ----------------
    nextDir = Operations::randomHemisphereDirection(normal);
    weight = material.kd;
    isDelta = false;
}

// ---------------- Iluminación directa ----------------
Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
) {
    Pixel color = {0,0,0};

    // Si es espejo o dielectrico no hay iluminación directa
    bool esDeltaPuro = (material.isEspejoPuro() || material.isDielectric());
    if (esDeltaPuro) return color;

    bool existeLuzPuntual = hayLuzPuntual(lights);
    if (existeLuzPuntual){
        for (const auto& luz : lights) {
            // Origen del rayo de sombra desplazado para evitar auto-intersección
            Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
            Point lightSample;
            Direction toLight;
            float dist;
                
            // Luz puntual
            lightSample = luz.origen;
            toLight = Operations::sub(lightSample, shadowOrigin);
            dist = Operations::mod(toLight);
            toLight = Operations::norm(toLight);
            
            // Verificar sombras
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
                // Contribución de luz (puntual)
                Pixel contrib = luz.calcularLuzBSDF(origen, normal, material, toLight, wo);

                // multiplicar por kd para preservar el color del material
                color.r += contrib.r * material.kd.R;
                color.g += contrib.g * material.kd.G;
                color.b += contrib.b * material.kd.B;
            }
        }
    }
    else{
        color = {0,0,0};
    }    
    return color;
}



// ---------------- Rebote indirecto ----------------
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
    if (depth >= maxBounces) return {0,0,0};

    Pixel colorDirecto = calcularIluminacionDirecta(origen, normal, material, wo, primitives, lights);

    Direction nextDir;
    Color weight;
    bool isDelta = false;
    muestrearDireccionRebote(material, normal, wo, nextDir, weight, isDelta);

    float maxWeight = std::max({weight.R, weight.G, weight.B, 0.05f});

    // Russian Roulette para evitar caminos muy largos de difusos
    if (!isDelta /*&& depth > 2*/) {
        float xi = static_cast<float>(rand()) / RAND_MAX;
        if (xi > maxWeight) return colorDirecto;
        weight.R /= maxWeight;
        weight.G /= maxWeight;
        weight.B /= maxWeight;
    }

    Point nextOrigin = Operations::add(origen, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());
    Interseccion inter = calcularInterseccionMasCercana(nextRay, primitives);

    Pixel indirecto = {0,0,0};
    if (inter.hayInterseccion && inter.primitiva) {
        if (inter.primitiva->emitsLight()) {
            Plane* emisiva = static_cast<Plane*>(inter.primitiva);
            Pixel emision = emisiva->getAreaLight().calcularRadianciaRectangular();
            
            indirecto = emision;
        }else{
            Direction newWo = Operations::mult(nextRay.direction_.toDirection(), -1.0f);
            Pixel rec = calcularReboteIndirecto(inter.punto, inter.normal, inter.primitiva->getMaterial(),
                                                newWo, primitives, lights, depth+1, maxBounces);
            indirecto.r = rec.r * weight.R;
            indirecto.g = rec.g * weight.G;
            indirecto.b = rec.b * weight.B;
        }
    }

    Pixel total;
    total.r = colorDirecto.r + indirecto.r;
    total.g = colorDirecto.g + indirecto.g;
    total.b = colorDirecto.b + indirecto.b;

    return total;
}
// ---------------- Iluminación global unificada ----------------
Pixel calcularIluminacionGlobal(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces
) {
    Pixel colorTotal = {0,0,0};
    const Material& material = inter.primitiva->getMaterial();

    // --- Emisión propia ---
    if (inter.primitiva->emitsLight()) {
        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
        Pixel emision = emisiva->getAreaLight().calcularRadianciaRectangular();
        
        return emision;
    }

    // --- Iluminación directa: solo si no es delta---
    
    if (!material.isEspejoPuro() && !material.isDielectric()) {
        Pixel directa = calcularIluminacionDirecta(inter.punto, inter.normal, material, wo, primitives, luces);
        // En principio no hace fatlta multiplicar por kd para preservar color
        //directa.r *= material.kd.R;
        //directa.g *= material.kd.G;
        //directa.b *= material.kd.B;

        colorTotal.r += directa.r;
        colorTotal.g += directa.g;
        colorTotal.b += directa.b;
    }

    // --- Rebote indirecto ---
    if (incluirIndirecta && depth < maxBounces) {
        Pixel indirecta = calcularReboteIndirecto(inter.punto, inter.normal, material,
                                                  wo, primitives, luces, depth, maxBounces);
        colorTotal.r += indirecta.r;
        colorTotal.g += indirecta.g;
        colorTotal.b += indirecta.b;
    }

    return colorTotal;
}
