#pragma once

#include "Point.hpp"
#include "Direction.hpp"
#include "Operations.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Primitive.hpp"
#include "Matrix.hpp"
#include "Light.hpp"
#include "Photon.hpp"
#include "PhotonMapper.hpp"
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

    Image generateImage(const std::vector<Primitive*>& primitives);
    Image generateImageMultipleRays(const std::vector<Primitive*>& primitives, int numRays);
    Image generateImageWithLightingOneRay(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights);
    Image generateImageWithLightingMultipleRays(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights, int numRays);
    Image generateImageWithLightingMultipleRaysConRebotes(const std::vector<Primitive*>& primitives, const std::vector<Light>& lights, int numRays, int maxBounces); 
    Image generateImageWithLightingMultipleRaysConFotones(
        const std::vector<Primitive*>& primitives,
        const std::vector<Light>& lights,
        PhotonMapper* photonMapper,
        int numRays,
        int maxBounces,
        float pm_radius,
        int pm_nPhotons
    );
    
};