#pragma once

#include "Scene.hpp"
#include "Photon.hpp"
#include "Vector3.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Material.hpp"
#include <vector>
#include <list>

class Primitive;

// -----------------------------------------------------------------------------
// Estructura de Intersección
// -----------------------------------------------------------------------------
struct InterseccionData {
    bool hayInterseccion = false;
    float t = 1e30f;
    const Primitive* primitiva = nullptr;
    Point punto;
    Direction normal;
};

// -----------------------------------------------------------------------------
// Declaración adelantada de KD-Tree para fotones
// -----------------------------------------------------------------------------
namespace nn {
    template<typename T, size_t K, typename Accessor>
    class KDTree;
}
using PhotonMap = nn::KDTree<Photon, 3, PhotonAxisPosition>;

// -----------------------------------------------------------------------------
// Enumeración para los kernels de estimación de radiancia
// -----------------------------------------------------------------------------
enum KERNEL {
    BOX,
    CONE,
    GAUSSIAN
};

// -----------------------------------------------------------------------------
// Clase PhotonMapper
// -----------------------------------------------------------------------------
class PhotonMapper {
public:
    PhotonMapper(Scene* scene);
    ~PhotonMapper();

    // Emisión y construcción del mapa de fotones
    void emitPhotons(int nPhotons);
    void buildPhotonMap();

    // Estimación de radiancia
    Color estimateRadiance(
        const Point& p,
        const Direction& normal,
        const Direction& wo,
        const Material& material,
        float maxRadius,
        int nPhotonsToFind,
        KERNEL kernelType
    ) const;

    // Información útil para debug
    size_t getTotalStoredPhotons() const {
        return photons_.size();
    }

private:
    Scene* scene_;
    std::vector<Photon> photons_;

    // KD-tree alojado en el heap
    PhotonMap* map_ = nullptr;

    // Trazado de fotones
    void tracePhoton(Ray& ray, Color currentFlux, int depth);

    // Muestreo de dirección
    void samplePhotonDirection(
        const Material& material,
        const Direction& normal,
        const Direction& incidentDir,
        Direction& nextDir,
        Color& throughput
    );

    // Intersección
    InterseccionData calcularInterseccionMasCercana_PM(
        Ray& ray,
        const std::vector<Primitive*>& primitives
    ) const;
};
