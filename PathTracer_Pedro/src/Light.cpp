#include "Light.hpp"

// Constructor
PointLight::PointLight(const Vector3& pos, const Color& inten)
    : position(pos), intensity(inten) {}

// Obtener la posición de la luz
Vector3 PointLight::getPosition() const {
    return position;
}

// Obtener la intensidad de la luz
Color PointLight::getIntensity() const {
    return intensity;
}
