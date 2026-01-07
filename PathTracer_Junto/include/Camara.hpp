#pragma once

#include <random>
#include "Point.hpp"
#include "Direction.hpp"
#include "Operations.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Primitive.hpp"
#include "Matrix.hpp"
#include "Light.hpp"

class Camera {
public:
    Point origen;
    Direction left;
    Direction up;
    Direction forward;

    int pixelWidth;    // Número de píxeles a lo ancho
    int pixelHeight;   // Número de píxeles a lo alto
    const float anchoPlano = 2.0f;
    const float altoPlano = 2.0f;
    float onepixelWidth;
    float onepixelHeight;

    Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp);

    // Método para generar un rayo hacia el centro de un píxel dado (i, j)
    Ray generateRay(int i, int j, bool multipleSamples);

    // Método para generar un rayo hacia el centro de un píxel dado (i, j)
    Ray generateRayHilos(int i, int j, bool multipleSamples,std::mt19937& rng);

    Image generateImage(const std::vector<Primitive*>& primitives);
    Image generateImageMultipleRays(const std::vector<Primitive*>& primitives, int numRays);
    Image generateImageWithLightingOneRay(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights);
    Image generateImageWithLightingMultipleRays(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights, int numRays);
    Image generateImageWithLightingMultipleRaysConRebotes(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights, int numRays, int maxBounces); 

    // Nuevos métodos con aceleración BVH (opcionales, no tocan los anteriores)
    Image generateImageWithLightingBVH(const class SceneBVH& sceneBVH, const std::vector<Light>& lights, int numRays, int maxBounces);
#ifdef USE_EMBREE
    Image generateImageWithLightingEmbree(const class SceneEmbree& sceneEmbree, const std::vector<Light>& lights, int numRays, int maxBounces);
#endif
    
};