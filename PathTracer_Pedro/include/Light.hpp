#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "Vector3.hpp"
#include "Color.hpp"

class PointLight {
public:
    Vector3 position; // Posición de la luz
    Color intensity;  // Potencia de la luz (tripleta RGB)

    // Constructor
    PointLight(const Vector3& pos, const Color& inten);

    // Métodos
    Vector3 getPosition() const;   // Obtener la posición de la luz
    Color getIntensity() const;    // Obtener la intensidad de la luz
};

#endif // POINTLIGHT_HPP
