#include "SceneBVH.hpp"
#include <iostream>

SceneBVH::SceneBVH(const Scene& scene)
    : sceneRef(scene), bvh(nullptr)
{
    // Obtener todas las primitivas de la escena
    auto primitives = sceneRef.getPrimitives();
    
    if (!primitives.empty()) {
        // Construir el BVH con las primitivas
        bvh = new BVHNode(primitives, 0);
        std::cout << "[BVH] Construido exitosamente con " 
                  << primitives.size() << " primitivas\n";
    } else {
        std::cout << "[BVH] Advertencia: Escena vacía, no se construyó BVH\n";
    }
}

SceneBVH::~SceneBVH()
{
    // Liberar memoria del BVH (si es necesario, implementar en BVHNode si no lo hace)
    if (bvh) {
        delete bvh;
        bvh = nullptr;
    }
}

Interseccion SceneBVH::intersect(Ray& ray) const
{
    Interseccion inter;
    inter.hayInterseccion = false;
    inter.t = std::numeric_limits<float>::max();
    inter.primitiva = nullptr;

    // Si no hay BVH, retornar intersección vacía
    if (!bvh) return inter;

    // Realizar búsqueda en el BVH
    Primitive* hit = nullptr;
    if (bvh->intersect(ray, inter.t, hit)) {
        inter.hayInterseccion = true;
        inter.primitiva = hit;
        
        // Calcular punto de intersección
        inter.punto = Operations::add(
            ray.origin_.toPoint(),
            Operations::mult(ray.direction_.toDirection(), inter.t)
        );
        
        // Obtener normal en el punto de intersección
        inter.normal = hit->getPointNormal(inter.punto);
    }

    return inter;
}

bool SceneBVH::intersectShadow(Ray& ray, float maxDist) const
{
    if (!bvh) return false;
    
    float t = maxDist;
    Primitive* hit = nullptr;
    return bvh->intersect(ray, t, hit);
}

const Scene& SceneBVH::getScene() const
{
    return sceneRef;
}
