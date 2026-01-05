#ifndef RENDERIZADOR_EMBREE_HPP
#define RENDERIZADOR_EMBREE_HPP

#include <vector>

#include "Point.hpp"
#include "Direction.hpp"
#include "Ray.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Primitive.hpp"

#ifdef USE_EMBREE
#include "SceneEmbree.hpp"
#endif

#ifdef USE_EMBREE

// ------------------------------------------------------------
// Sombras usando Embree
// ------------------------------------------------------------
bool estaEnSombraEmbree(
    const Point& punto,
    const Direction& normal,
    const Light& luz,
    const SceneEmbree& sceneEmbree
);

// ------------------------------------------------------------
// Iluminación directa (Embree)
// ------------------------------------------------------------
Pixel calcularIluminacionDirectaEmbree(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const SceneEmbree& sceneEmbree,
    const std::vector<Light>& lights
);

// ------------------------------------------------------------
// Iluminación global (directa + indirecta)
// ------------------------------------------------------------
Pixel calcularIluminacionGlobalEmbree(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const SceneEmbree& sceneEmbree,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces
);

// ------------------------------------------------------------
// Rebote indirecto recursivo
// ------------------------------------------------------------
Pixel calcularReboteIndirectoEmbree(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const SceneEmbree& sceneEmbree,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces
);

#endif // USE_EMBREE

#endif // RENDERIZADOR_EMBREE_HPP
