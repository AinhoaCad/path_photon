#include <cstdlib> // Para rand()
#include <random>
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

float randomOffsetHilos(float pixelSize, std::mt19937& rng) {
    // Offset uniforme dentro del píxel: [-0.5, 0.5] * pixelSize
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    return dist(rng) * pixelSize;
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

// ---------------------------------------------------------
// Muestreo de nueva dirección según material (Monte Carlo)
// ---------------------------------------------------------
// En esta función resolvemos UN paso del estimador de Monte Carlo
// para la iluminación indirecta en un vértice del camino.
//
// Ecuación de Render (forma integral):
//   L_o = ∫_Ω f_r(ω_i, ω_o) · L_i(ω_i) · cosθ dω_i
//
// Estimador Monte Carlo con una muestra:
//   L_o ≈ [ (f_r · cosθ) / p(ω_i) ] · L_i(ω_i)
//
// Esta función devuelve el término entre corchetes:
//   weight = (f_r · cosθ) / p(ω_i)
//
// Además:
//   - nextDir  → dirección incidente muestreada ω_i
//   - isDelta  → indica si el material es delta (espejo / dieléctrico)
void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
) {
    // Dirección incidente hacia la superficie
    Direction incident = Operations::mult(wo, -1.0f);

    // =====================================================
    // ESPEJO PURO (BRDF Delta)
    // =====================================================
    // En un material especular ideal la BRDF es una delta:
    // solo existe una única dirección válida (reflexión perfecta).
    //
    // La integral desaparece y no hay PDF continua.
    // El estimador se reduce directamente al coeficiente especular:
    //   weight = k_s
    if (material.isEspejoPuro()) {
        nextDir = Operations::reflect(incident, normal);
        weight  = material.ks;
        isDelta = true;
        return;
    }

    // =====================================================
    // DIELÉCTRICO (Reflexión / Refracción Delta)
    // =====================================================
    if (material.isDielectric()) {

        Direction n = normal;
        bool entering = (Operations::dot(incident, n) < 0.0f);

        float etaI = 1.0f;          // Índice del aire
        float etaT = material.ior;  // Índice del material

        Vector3 refracted;
        bool canRefract = Operations::refract(
            incident.toVector(),
            n.toVector(),
            etaI,
            etaT,
            refracted
        );

        // ---------------- Reflexión Interna Total ----------------
        if (!canRefract) {
            Direction reflectNormal = entering ? n : Operations::mult(n, -1.0f);
            nextDir = Operations::reflect(incident, reflectNormal);
            weight  = material.ks;
            isDelta = true;
            return;
        }

        // Ruleta Rusa entre reflexión y refracción.
        // Dividir el peso por la probabilidad mantiene el estimador insesgado:
        //   E[X] = P · (X / P) = X
        float Er = std::max({material.ks.R, material.ks.G, material.ks.B});
        float Et = std::max({material.kt.R, material.kt.G, material.kt.B});
        float sumE = std::max(Er + Et, 1.0f);

        float Pr = Er / sumE;  // Probabilidad de reflexión
        float Pt = Et / sumE;  // Probabilidad de refracción

        float xi = static_cast<float>(rand()) / RAND_MAX;

        if (xi < Pr) {
            // ---------------- Reflexión ----------------
            Direction reflectNormal = entering ? n : Operations::mult(n, -1.0f);
            nextDir = Operations::reflect(incident, reflectNormal);
            weight  = material.ks / std::max(Pr, 1e-6f);
        } else {
            // ---------------- Refracción ----------------
            // Corrección de radiancia por cambio de medio:
            //   (η_i / η_t)^2
            float etaRatio = entering ? (etaI / etaT) : (etaT / etaI);
            float etaCorr  = etaRatio * etaRatio;

            nextDir = refracted;
            weight  = (material.kt * etaCorr) / std::max(Pt, 1e-6f);
        }

        isDelta = true;
        return;
    }

    // =====================================================
    // DIFUSO PURO (Lambertiano)
    // =====================================================
    // Usamos muestreo coseno-ponderado:
    //   f_r = k_d / π
    //   p(ω_i) = cosθ / π
    //
    // Sustituyendo en el estimador:
    //   weight = (f_r · cosθ) / p(ω_i)
    //          = [ (k_d / π) · cosθ ] / [ cosθ / π ]
    //          = k_d
    //
    // El coseno y π se cancelan exactamente.
    nextDir = Operations::randomHemisphereDirection(normal);
    weight  = material.kd;
    isDelta = false;
}

// ---------------- Iluminación directa ----------------
// Esta función implementa Next Event Estimation (NEE) para calcular la luz directa
// de luces puntuales visibles desde un punto de intersección.
//
// Ecuación de la luz directa (adaptación de la ecuación de render):
// L_o(x, ω_o) ≈ Σ_l V(x, c_l) · (I_l / ||c_l - x||^2) · f_r(x, ω_l, ω_o) · max(0, n · ω_l)
//
// Donde:
// - L_o(x, ω_o) → Radiancia saliente desde x hacia la cámara (ω_o)
// - Σ_l         → Suma sobre todas las luces puntuales visibles
// - V(x, c_l)   → Visibilidad: 1 si la luz l es visible desde x, 0 si está bloqueada (sombras)
// - I_l         → Intensidad de la luz puntual l
// - ||c_l - x||^2 → Atenuación por distancia
// - f_r(x, ω_l, ω_o) → BRDF en x, describe cómo la luz se refleja
// - max(0, n · ω_l)  → Coseno del ángulo de incidencia, factor Lambertiano
Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
) {
    Pixel color = {0,0,0};

    // ---------------- Paso 0: descartamos materiales delta ---------------
    // Los materiales espejo o dieléctricos delta no contribuyen a iluminación directa
    bool esDeltaPuro = (material.isEspejoPuro() || material.isDielectric());
    if (esDeltaPuro) return color;

    // ---------------- Paso 1: bucle sobre todas las luces puntuales ---------------
    bool existeLuzPuntual = hayLuzPuntual(lights);
    if (existeLuzPuntual){
        for (const auto& luz : lights) {
            // ---------------- Paso 1a: origen del rayo de sombra ----------------
            // Desplazamos ligeramente el origen para evitar auto-intersección
            // Este rayo servirá para calcular el término V(x, c_l)
            Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
            Point lightSample = luz.origen;

            // ---------------- Paso 1b: dirección y distancia a la luz --------------
            // ω_l = dirección normalizada desde x hacia la luz
            Direction toLight = Operations::sub(lightSample, shadowOrigin);
            float dist = Operations::mod(toLight);  // ||c_l - x||
            toLight = Operations::norm(toLight);

            // ---------------- Paso 1c: cálculo de visibilidad V(x, c_l) ----------
            // Lanzamos un rayo de sombra y comprobamos si algo bloquea la luz
            Ray shadowRay(shadowOrigin.toVector(), toLight.toVector());
            bool inShadow = false;
            for (auto& prim : primitives) {
                float shadowT;
                if (prim->intersect(shadowRay, shadowT) && shadowT > 0.001f && shadowT < dist) {
                    inShadow = true; // bloqueado → V = 0
                    break;
                }
            }

            // ---------------- Paso 2: si la luz es visible -----------------------
            if (!inShadow) {
                // Calcular contribución de la luz: f_r · I_l / r^2 · cosθ
                Pixel contrib = luz.calcularLuzBSDF(origen, normal, material, toLight, wo);

                // Multiplicamos por el color del material kd 
                color.r += contrib.r * material.kd.R;  
                color.g += contrib.g * material.kd.G;
                color.b += contrib.b * material.kd.B;

                // I_l / ||c_l - x||^2 ya está incorporado en calcularLuzBSDF
                // max(0, n·ω_l) también se aplica dentro de la función BSDF
            }
        }
    }

    // ---------------- Paso 3: retorno de L_o(x, ω_o) ----------------------
    return color;
}


// ---------------- Rebote indirecto ----------------
// ---------------------------------------------------------
// Rebote indirecto (Path Tracing recursivo)
// ---------------------------------------------------------
// Esta función evalúa la iluminación indirecta L_ind en un punto
// usando Path Tracing con Monte Carlo.
//
// Implementa la parte:
//   ∫_Ω f_r(x, ω_i, ω_o) · L_i(x, ω_i) · cosθ dω_i
//
// aproximada mediante:
//   UNA muestra por vértice + recursión
Pixel calcularReboteIndirecto(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces,
    std::mt19937& rng
) {
    // ---------------- Condición de parada ----------------
    // Limitamos la profundidad del camino para evitar recursión infinita
    if (depth >= maxBounces)
        return {0, 0, 0};

    // ---------------- Iluminación directa ----------------
    // Esta parte NO es Monte Carlo hemisférico:
    // corresponde al término de luces explícitas (NEE / shadow rays)
    //
    // Se suma aquí porque, desde el punto de vista de la ecuación,
    // la radiancia entrante puede venir:
    //  - directamente de luces
    //  - indirectamente de otras superficies
    Pixel colorDirecto = calcularIluminacionDirecta(
        origen, normal, material, wo, primitives, lights
    );

    // ---------------- Muestreo Monte Carlo ----------------
    // Aquí construimos UNA muestra del estimador MC:
    //  - Elegimos una dirección ω_i
    //  - Calculamos el peso (f_r · cosθ) / p(ω_i)
    Direction nextDir;
    Color weight;
    bool isDelta = false;

    muestrearDireccionRebote(
        material,
        normal,
        wo,
        nextDir,
        weight,
        isDelta
    );

    // ---------------- Ruleta Rusa ----------------
    // Para materiales NO delta (difusos / glossy),
    // aplicamos Russian Roulette para terminar caminos largos.
    //
    // La probabilidad se basa en la energía del peso.
    // Si el camino sobrevive, dividimos por la probabilidad
    // para mantener el estimador insesgado.
    float maxWeight = std::max({weight.R, weight.G, weight.B, 0.05f});

    if (!isDelta) {
        float xi = static_cast<float>(rand()) / RAND_MAX;
        if (xi > maxWeight)
            return colorDirecto; // Camino terminado

        // Corrección de energía (X / P)
        weight.R /= maxWeight;
        weight.G /= maxWeight;
        weight.B /= maxWeight;
    }

    // ---------------- Lanzamiento del siguiente rayo ----------------
    // Esto representa L_i(x, ω_i) en la ecuación:
    // la radiancia que llega desde la dirección muestreada
    Point nextOrigin = Operations::add(
        origen,
        Operations::mult(nextDir, 0.001f) // evitar auto-intersección
    );

    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());
    Interseccion inter = calcularInterseccionMasCercana(nextRay, primitives);

    Pixel indirecto = {0, 0, 0};

    if (inter.hayInterseccion && inter.primitiva) {

        // ---------------- Caso: alcanzamos una luz ----------------
        // L_i es directamente la emisión de la luz
        if (inter.primitiva->emitsLight()) {

            Plane* emisiva = static_cast<Plane*>(inter.primitiva);
            Pixel emision = emisiva->getAreaLight().calcularRadianciaRectangular();

            // IMPORTANTE:
            // Aquí NO multiplicamos por el peso todavía.
            // El peso se aplica al volver de la recursión.
            indirecto = emision;

        } else {
            // ---------------- Caso: otra superficie ----------------
            // Evaluamos recursivamente L_o en el siguiente punto
            Direction newWo = Operations::mult(
                nextRay.direction_.toDirection(),
                -1.0f
            );

            Pixel rec = calcularReboteIndirecto(
                inter.punto,
                inter.normal,
                inter.primitiva->getMaterial(),
                newWo,
                primitives,
                lights,
                depth + 1,
                maxBounces,
                rng
            );

            // ---------------- Aplicación del peso ----------------
            // Aquí es donde se evalúa el estimador MC completo:
            //
            //   L_ind ≈ weight · L_i
            //
            indirecto.r = rec.r * weight.R;
            indirecto.g = rec.g * weight.G;
            indirecto.b = rec.b * weight.B;
        }
    }

    // ---------------- Suma final ----------------
    // L_o = L_directa + L_indirecta
    Pixel total;
    total.r = colorDirecto.r + indirecto.r;
    total.g = colorDirecto.g + indirecto.g;
    total.b = colorDirecto.b + indirecto.b;

    return total;
}

// ---------------- Iluminación global unificada ----------------

//Ecuacion de Render : Lo(x, wo) = Le(x, wo) + ∫_Ω f_r(x, wi, wo) Li(x, wi) |n·wi| dwi
Pixel calcularIluminacionGlobal(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces,
    std::mt19937& rng
) {
    Pixel colorTotal = {0,0,0};
    const Material& material = inter.primitiva->getMaterial();

    // --- Emisión propia ---
    // Termino Le(x, wo)
    if (inter.primitiva->emitsLight()) {
        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
        Pixel emision = emisiva->getAreaLight().calcularRadianciaRectangular();
        
        return emision;
    }

    //Separamos la integral : ∫_Ω = ∫_Ω_luces_directa + ∫_Ω_luces_indirectas

    // --- Iluminación directa: solo si no es delta---
    //Parte de la ecuacion ∫_Ω_luces_directa f_r(x, wi, wo) Li(x, wi) |n·wi| dwi

    /*IMPORTANTE: Descartamos materiales deltas porque la probabilidad de que un rayo lanzado directamente a una luz puntual coincida 
    con la única dirección de reflexión/refracción perfecta es cero. Estos materiales se calculan exclusivamente 
    a través del rebote indirecto. */
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
    //Parte de la ecuacion ∫_Ω_luces_indirectas f_r(x, wi, wo) Li(x, wi) |n·wi| dwi
    if (incluirIndirecta && depth < maxBounces) {
        Pixel indirecta = calcularReboteIndirecto(inter.punto, inter.normal, material,
                                                  wo, primitives, luces, depth, maxBounces, rng);
        colorTotal.r += indirecta.r;
        colorTotal.g += indirecta.g;
        colorTotal.b += indirecta.b;
    }

    return colorTotal;
}
