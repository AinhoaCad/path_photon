#pragma once
#include "Vector3.hpp"
#include "Image.hpp" // Asumo que 'Pixel' se define aquí.

// -----------------------------------------------------------------------------
// Estructura que representa un fotón individual en el Photon Map
// -----------------------------------------------------------------------------
struct Photon {
    Vector3 position;           // Posición 3D del fotón en el espacio
    Vector3 directionIncidente; // Dirección desde la que llegó el fotón (útil para calcular BRDF)
    Pixel flux;                 // Energía o color del fotón (Phi), usado para estimar radiancia

    // Constructor (Opcional, pero recomendado)
    Photon(const Vector3& pos, const Vector3& dir, const Pixel& f)
        : position(pos), directionIncidente(dir), flux(f) {}

    // Constructor por defecto
    Photon() = default;

    // -------------------------------------------------------------------------
    // Función que devuelve la coordenada del fotón en el eje i (0=x,1=y,2=z)
    // Necesario para la estructura KD-Tree.
    // -------------------------------------------------------------------------
    float positionAt(std::size_t i) const {
        if (i == 0) return position.x; // coordenada X
        if (i == 1) return position.y; // coordenada Y
        return position.z;             // coordenada Z
    }
};

// -----------------------------------------------------------------------------
// Functor que permite al KD-Tree acceder a la posición de un fotón
// -----------------------------------------------------------------------------
struct PhotonAxisPosition {
    float operator()(const Photon& p, std::size_t i) const {
        return p.positionAt(i); // Devuelve la coordenada del fotón para el eje i
    }
};
