#pragma once
#include "Scene.hpp"
#include "BVHNode.hpp"
#include "Renderizador.hpp"
#include <iostream>
#include <limits>

class SceneBVH {
public:
    // Constructor: toma una Scene por referencia y construye el BVH
    SceneBVH(const Scene& scene);

    // Destructor: libera la memoria del BVH
    ~SceneBVH();

    // Realiza intersección con el BVH
    Interseccion intersect(Ray& ray) const;

    // Verifica si un rayo intersecta algo antes de una distancia (para sombras)
    bool intersectShadow(Ray& ray, float maxDist) const;

    // Obtiene referencia a la escena
    const Scene& getScene() const;

private:
    const Scene& sceneRef;  // Referencia a la Scene original (sin poseer)
    BVHNode* bvh;           // Árbol BVH (posee la memoria)
};
