#include "scene.hpp"


Scene::Scene(Camera _sensor, vector<Primitive*> _primitives, vector<LightSource*> _Lightsources): sensor(_sensor),primitives(_primitives),LightSources(_Lightsources)
{
}

vector<Primitive*> Scene::getPrimitives()
{
    return primitives;
}

Camera Scene::getCamera()
{
    return sensor;
}

vector<LightSource*> Scene::getLightSources() {
    return LightSources;
}
