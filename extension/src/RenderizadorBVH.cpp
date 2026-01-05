#include "Renderizador.hpp"
#include "RenderizadorBVH.hpp"
#ifdef USE_EMBREE
#include "SceneEmbree.hpp"
#endif
#include "Operations.hpp"
#include "Plane.hpp"
#include "Light.hpp"
#include <iostream>
#include <cstdlib>
#include <cmath>


#ifdef USE_EMBREE
// ========== Sombras con Embree + fallback ==========
bool estaEnSombraEmbree(const Point& punto, const Direction& normal, const Light& luz, const SceneEmbree& sceneEmbree)
{
    const float bias = 0.001f;
    Point shadowOrigin = Operations::add(punto, Operations::mult(normal, bias));

    Point lightSample = luz.origen;
    Direction toLight = Operations::sub(lightSample, shadowOrigin);
    float maxDist = Operations::mod(toLight);
    toLight = Operations::norm(toLight);

    Ray shadowRay(shadowOrigin.toVector(), toLight.toVector());
    return sceneEmbree.intersectShadow(shadowRay, maxDist);
}

// Iluminación directa usando Embree (equivalente a calcularIluminacionDirectaBVH)
Pixel calcularIluminacionDirectaEmbree(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const SceneEmbree& sceneEmbree,
    const std::vector<Light>& lights
) {
    Pixel color = {0,0,0};

    if (material.isEspejoPuro() || material.isDielectric()) return color;

    for (const auto& luz : lights) {
        Point shadowOrigin = Operations::add(origen, Operations::mult(normal, 0.001f));
        Point lightSample = luz.origen;
        Direction toLight = Operations::sub(lightSample, shadowOrigin);
        float dist = Operations::mod(toLight);
        toLight = Operations::norm(toLight);

        Ray shadowRay(shadowOrigin.toVector(), toLight.toVector());
        bool inShadow = sceneEmbree.intersectShadow(shadowRay, dist);
        if (inShadow) continue;
        Pixel contrib = luz.calcularLuzBSDF(origen, normal, material, toLight, wo);
        color.r += contrib.r * material.kd.R;
        color.g += contrib.g * material.kd.G;
        color.b += contrib.b * material.kd.B;
    }

    return color;
}

Pixel calcularIluminacionGlobalEmbree(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const SceneEmbree& sceneEmbree,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces
)
{
    if (!inter.primitiva) {
        return {0, 0, 0};
    }

    Pixel colorTotal = {0, 0, 0};
    const Material& material = inter.primitiva->getMaterial();

    if (inter.primitiva->emitsLight()) {
        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
        return emisiva->getAreaLight().calcularRadianciaRectangular();
    }

    if (!material.isEspejoPuro() && !material.isDielectric()) {
        Pixel directa = calcularIluminacionDirectaEmbree(inter.punto, inter.normal, material, wo, sceneEmbree, luces);
        colorTotal.r += directa.r;
        colorTotal.g += directa.g;
        colorTotal.b += directa.b;
    }

    if (incluirIndirecta && depth < maxBounces) {
        Pixel indirecta = calcularReboteIndirectoEmbree(
            inter.punto,
            inter.normal,
            material,
            wo,
            sceneEmbree,
            luces,
            depth,
            maxBounces
        );
        colorTotal.r += indirecta.r;
        colorTotal.g += indirecta.g;
        colorTotal.b += indirecta.b;
    }

    return colorTotal;
}

Pixel calcularReboteIndirectoEmbree(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const SceneEmbree& sceneEmbree,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces
)
{
    if (depth >= maxBounces) {
        return {0,0,0};
    }

    Pixel colorDirecto = calcularIluminacionDirectaEmbree(origen, normal, material, wo, sceneEmbree, lights);

    Direction nextDir;
    Color weight;
    bool isDelta = false;
    muestrearDireccionRebote(material, normal, wo, nextDir, weight, isDelta);

    Point nextOrigin = Operations::add(origen, Operations::mult(nextDir, 0.001f));
    Ray nextRay(nextOrigin.toVector(), nextDir.toVector());

    Interseccion inter = sceneEmbree.intersect(nextRay);

    Pixel indirecto = {0,0,0};
    if (inter.hayInterseccion && inter.primitiva) {
        if (inter.primitiva->emitsLight()) {
            Plane* emisiva = static_cast<Plane*>(inter.primitiva);
            Pixel emision = emisiva->getAreaLight().calcularRadianciaRectangular();
            indirecto = emision;
        } else {
            Direction newWo = Operations::mult(nextRay.direction_.toDirection(), -1.0f);
            Pixel rec = calcularReboteIndirectoEmbree(
                inter.punto, inter.normal, inter.primitiva->getMaterial(),
                newWo, sceneEmbree, lights, depth+1, maxBounces
            );
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
#endif // USE_EMBREE
