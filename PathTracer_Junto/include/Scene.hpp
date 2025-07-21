#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Light.hpp"

class Scene {
public:
    std::vector<Sphere> spheres;      // Esferas en la escena
    std::vector<Plane> planes;        // Planos en la escena
    std::vector<PointLight> lights;   // Luces en la escena

    // Constructor por defecto
    Scene() {}

    // Métodos para agregar elementos a la escena
    void addSphere(const Sphere& sphere);
    void addPlane(const Plane& plane);
    void addLight(const PointLight& light);

    // Métodos para obtener los elementos de la escena
    const std::vector<Sphere>& getSpheres() const;
    const std::vector<Plane>& getPlanes() const;
    const std::vector<PointLight>& getLights() const;
};

#endif // SCENE_HPP