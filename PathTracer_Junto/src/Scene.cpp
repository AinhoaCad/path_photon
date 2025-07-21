#include "Scene.hpp"

void Scene::addSphere(const Sphere& sphere) {
    spheres.push_back(sphere);
}

void Scene::addPlane(const Plane& plane) {
    planes.push_back(plane);
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