#include "Scene.hpp"

void Scene::addSphere(const Sphere& sphere) {
    spheres.push_back(sphere);
}

void Scene::addPlane(const Plane& plane) {
    planes.push_back(plane);

    // Si el plano tiene luz asociada, añadirla también al vector de luces
    if (plane.emitsLight()) {
        lights.push_back(plane.getAreaLight());
    }
}

void Scene::addLight(const Light& light) {
    lights.push_back(light);
}

const std::vector<Sphere>& Scene::getSpheres() const {
    return spheres;
}

const std::vector<Plane>& Scene::getPlanes() const {
    return planes;
}

const std::vector<Light>& Scene::getLights() const {
    return lights;
}

// Método para obtener el número de primitivas en la escena
size_t Scene::getNumPrimitives() const {
    return spheres.size() + planes.size();
}

// Método para obtener las primitivas de la escena
const std::vector<Primitive*> Scene::getPrimitives() const {
    static std::vector<Primitive*> primitives;
    primitives.clear();
    for (const auto& sphere : spheres) {
        primitives.push_back(const_cast<Sphere*>(&sphere));
    }
    for (const auto& plane : planes) {
        primitives.push_back(const_cast<Plane*>(&plane));
    }
    return primitives;
}