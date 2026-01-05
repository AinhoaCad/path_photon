#pragma once

#include "Ray.hpp"
#include "Primitive.hpp"
#include "Image.hpp"
#include <vector>
#include "Light.hpp"
#include "Plane.hpp"
#include "Material.hpp"
#include "Photon.hpp"
#include "PhotonMapper.hpp" // Incluye InterseccionData y KERNEL

// ----------------- Funciones auxiliares -----------------

// Desplazamiento aleatorio para muestreo de píxel
float randomOffset(float pixelSize);

// Comprueba si hay alguna luz puntual en la escena
bool hayLuzPuntual(const std::vector<Light>& lights);

// Calcula el color de la primitiva más cercana al rayo
Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives);

// Calcula el promedio de varios colores
Pixel promedioColores(const std::vector<Pixel>& colores);

// Calcula la intersección más cercana de un rayo con las primitivas
InterseccionData calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives);

// Comprueba si un punto está en sombra respecto a una luz
bool estaEnSombra(
    const Point& punto,
    const Direction& normal,
    const Light& luz,
    const std::vector<Primitive*>& primitives
);

// Calcula la iluminación total (directa + indirecta) en un punto
Pixel calcularIluminacion(
    const InterseccionData& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives
);

// Calcula la iluminación directa de un punto desde todas las luces
Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
);

// Muestreo de la dirección de rebote en Path Tracing
void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
);

// ----------------- Funciones para Photon Mapping -----------------

// Iluminación global usando Photon Mapping
Pixel calcularIluminacionGlobalConFotones(
    const InterseccionData& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives,
    const Direction& wo,
    PhotonMapper* photonMapper,
    float pm_radius,
    int pm_nPhotons,
    KERNEL pm_kernelType,
    int depth,
    int maxBounces,
    bool usarPhotonMappingParaDirecta = false
);

// Rebote indirecto usando Photon Mapping
Pixel calcularReboteIndirectoConFotones(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    PhotonMapper* photonMapper,
    float pm_radius,
    int pm_nPhotons,
    KERNEL pm_kernelType,
    int depth,
    int maxBounces,
    bool usarPhotonMappingParaDirecta = false
);
