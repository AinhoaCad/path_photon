#pragma once

#include <vector>
#include <cmath>
#include "Photon.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "Vector3.hpp"
#include "../../KDTree/kdtree.h"

// Alias para el KD-Tree de fotones
using PhotonMap = nn::KDTree<Photon, 3, PhotonAxisPosition>;

class PhotonMapper {
public:
    // Constructor con puntero a la escena
    PhotonMapper(Scene* scene);

    // Emitir nPhotons desde las luces puntuales
    void emitPhotons(int nPhotons);

    // Construir y obtener el KD-Tree con los fotones emitidos
    PhotonMap buildPhotonMap();

    // Obtener el mapa de fotones actual
    const PhotonMap& getPhotonMap() const { return map_; }

    // Funciones auxiliares para el Random Walk 
    void tracePhoton(Ray& ray, Color currentFlux, int depth); 
    // Muestrear el rebote del fotón 
    void samplePhotonDirection(
        const Material& material,
        const Direction& normal,
        const Direction& wo, // Dirección hacia la luz (saliente)
        Direction& nextDir,
        Color& throughput // El factor por el que se multiplica el flujo (k_d o k_s)
    );

    Color estimateRadiance(
        const Point& p,
        const Direction& normal,
        const Direction& wo,
        const Material& material, // <-- ¡Argumento añadido!
        float maxRadius,
        int nPhotonsToFind
    ) const;

private:
    Scene* scene_;                   // Puntero a la escena
    std::vector<Photon> photons_;    // Lista de fotones emitidos
    PhotonMap map_;                  // KD-Tree de fotones
};
