#pragma once

#ifdef USE_EMBREE

#include <embree4/rtcore.h>
#include <embree4/rtcore_ray.h>
#include <embree4/rtcore_common.h>
#include <vector>
#include <unordered_map>
#include "Scene.hpp"
#include "Renderizador.hpp"

// Acelerador basado en Embree: usa Embree para triángulos
// y hace fallback analítico para primitivas no triangulares.
class SceneEmbree {
public:
    explicit SceneEmbree(const Scene& scene);
    ~SceneEmbree();

    Interseccion intersect(Ray& ray) const;
    bool intersectShadow(Ray& ray, float maxDist) const;
    const Scene& getScene() const { return sceneRef; }

private:
    struct HitMap {
        std::vector<Primitive*> primById; // primID -> Primitive*
    };

    const Scene& sceneRef;
    RTCDevice device = nullptr;
    RTCScene rtcScene = nullptr;

    // Buffers que Embree usa; deben vivir mientras viva el scene.
    struct EmbreeVertex { float x, y, z; };
    struct EmbreeTri { unsigned int v0, v1, v2; };
    std::vector<EmbreeVertex> vertices;
    std::vector<EmbreeTri> indices;
    HitMap triMap; // geom único para todos los triángulos

    std::vector<Primitive*> fallbackPrims; // planos, esferas, etc.
};

#endif // USE_EMBREE
