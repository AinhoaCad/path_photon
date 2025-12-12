#pragma once

#include <vector>
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Light.hpp"

class Scene {
public:
    std::vector<Sphere> spheres;      // Esferas en la escena
    std::vector<Plane> planes;        // Planos en la escena
    std::vector<Light> lights;   // Luces en la escena

    // Constructor por defecto
    Scene() {}

    // Métodos para agregar elementos a la escena
    void addSphere(const Sphere& sphere);
    void addPlane(const Plane& plane);
    void addLight(const Light& light);

    // Método para obtener el número de primitivas en la escena
    size_t getNumPrimitives() const;

    // Método para obtener las primitivas de la escena
    const std::vector<Primitive*> getPrimitives() const;



    // Métodos para obtener los elementos de la escena
    const std::vector<Sphere>& getSpheres() const;
    const std::vector<Plane>& getPlanes() const;
    const std::vector<Light>& getLights() const;
};
