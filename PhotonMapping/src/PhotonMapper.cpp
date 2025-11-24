#include "PhotonMapper.hpp"
#include "../../PathTracer_Junto/include/Operations.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

// Constructor
PhotonMapper::PhotonMapper(Scene* scene)
    : scene_(scene) {
    srand(static_cast<unsigned>(time(nullptr))); // Semilla aleatoria
}

// Emitir nPhotons desde luces puntuales
void PhotonMapper::emitPhotons(int nPhotons) {
    photons_.clear();

    const auto& lights = scene_->getLights();
    if (lights.empty()) {
        std::cerr << "No hay luces en la escena para emitir fotones." << std::endl;
        return;
    }

    for (int i = 0; i < nPhotons; ++i) {
        // Seleccionar luz aleatoria
        int idx = rand() % lights.size();
        const Light& light = lights[idx];

        // Generar dirección aleatoria (muestra uniforme sobre esfera)
        float theta = acosf(1.0f - 2.0f * (rand() / (float)RAND_MAX));
        float phi = 2.0f * M_PI * (rand() / (float)RAND_MAX);

        Vector3 dir(
            sinf(theta) * cosf(phi),
            sinf(theta) * sinf(phi),
            cosf(theta)
        );

        // Crear rayo desde la luz
        Ray photonRay(light.origen.toVector(), dir);

        // Flujo inicial del fotón
        Vector3 flux(light.power.r, light.power.g, light.power.b);

        // ----- CAMINO DEL FOTÓN -----
        Vector3 pos = photonRay.origin();
        Vector3 incident = photonRay.direction();

        // Aquí puedes llamar a tu función de path tracing para rebotes
        // y ruleta rusa, actualizando pos y flux en cada rebote

        Photon p;
        p.position = pos;
        p.direction = incident;
        p.flux = flux;

        photons_.push_back(p);
    }
}

// Construir el KD-Tree con los fotones
PhotonMap PhotonMapper::buildPhotonMap() {
    map_ = PhotonMap(photons_, PhotonAxisPosition());
    return map_;
}
