#pragma once

#include <vector>
#include "Photon.hpp"
#include "../../PathTracer_Junto/include/Scene.hpp"
#include "../../PathTracer_Junto/include/Ray.hpp"
#include "../../PathTracer_Junto/include/Vector3.hpp"
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

private:
    Scene* scene_;                   // Puntero a la escena
    std::vector<Photon> photons_;    // Lista de fotones emitidos
    PhotonMap map_;                  // KD-Tree de fotones
};
