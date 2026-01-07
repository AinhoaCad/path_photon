#pragma once
#include<random>
#include "Ray.hpp"
#include "Primitive.hpp"
#include "Image.hpp"
#include <vector>
#include "Light.hpp"
#include "Plane.hpp"

// Funciones auxiliares para el renderizado


struct Interseccion {
    bool hayInterseccion;
    float t;
    Point punto;
    Direction normal;
    Primitive* primitiva;
};


float randomOffset(float pixelSize);

float randomOffsetHilos(float pixelSize,std::mt19937& rng);

bool hayLuzPuntual(const std::vector<Light>& lights);

Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives);

Pixel promedioColores(const std::vector<Pixel>& colores);

Interseccion calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives);

// Comprueba si un punto está en sombra respecto a una luz
bool estaEnSombra(const Point& punto, const Direction& normal, const Light& luz, const std::vector<Primitive*>& primitives);

// Calcula la iluminación total de un punto (difusa + emisiva)
Pixel calcularIluminacion(const Interseccion& inter, const std::vector<Light>& luces, const std::vector<Primitive*>& primitives);

// Calcula la iluminación directa en un punto desde todas las luces
Pixel calcularIluminacionDirecta(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights
);

// Calcula el rebote indirecto en un punto
void muestrearDireccionRebote(
    const Material& material,
    const Direction& normal,
    const Direction& wo,
    Direction& nextDir,
    Color& weight,
    bool& isDelta
);

Pixel calcularIluminacionGlobal(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const std::vector<Primitive*>& primitives,
    const Direction& wo,       
    bool incluirIndirecta,
    int depth,
    int maxBounces,
    std::mt19937& rng 
);

Pixel calcularReboteIndirecto(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const std::vector<Primitive*>& primitives,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces,
    std::mt19937& rng 
);
// ============= VERSIONES CON BVH (opcionales) =============
class SceneBVH; // Forward declaration
class SceneEmbree; // Forward declaration

// Verifica si un punto está en sombra usando BVH
bool estaEnSombraBVH(const Point& punto, const Direction& normal, const Light& luz, const SceneBVH& sceneBVH);

// Calcula iluminación global con BVH
Pixel calcularIluminacionGlobalBVH(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const SceneBVH& sceneBVH,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces
);

// Calcula rebote indirecto con BVH
Pixel calcularReboteIndirectoBVH(
    const Point& origen,
    const Direction& normal,
    const Material& material,
    const Direction& wo,
    const SceneBVH& sceneBVH,
    const std::vector<Light>& lights,
    int depth,
    int maxBounces
);

#ifdef USE_EMBREE
// Verifica si un punto está en sombra usando Embree
bool estaEnSombraEmbree(const Point& punto, const Direction& normal, const Light& luz, const SceneEmbree& sceneEmbree);

// Calcula iluminación global con Embree
Pixel calcularIluminacionGlobalEmbree(
    const Interseccion& inter,
    const std::vector<Light>& luces,
    const SceneEmbree& sceneEmbree,
    const Direction& wo,
    bool incluirIndirecta,
    int depth,
    int maxBounces
);

// Calcula rebote indirecto con Embree
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
#endif