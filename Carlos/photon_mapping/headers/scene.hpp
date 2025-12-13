#ifndef SCENE_HPP
#define SCENE_HPP
#include "point.hpp"
#include "direction.hpp"
#include "camera.hpp"
#include "primitive.hpp"
#include "lightSource.hpp"
#include <vector>

class Scene{
    Camera sensor;
    vector<Primitive*> primitives;
    vector<LightSource*> LightSources;
    public:
        Scene(Camera _sensor, vector<Primitive*> _primitives,vector<LightSource*> _Lightsources);
        vector<Primitive*> getPrimitives();
        Camera getCamera();
        vector<LightSource*> getLightSources();
};
#endif