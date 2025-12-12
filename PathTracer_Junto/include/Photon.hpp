#pragma once
#include "Vector3.hpp"
#include "Image.hpp"

// -----------------------------------------------------------------------------
// Estructura que representa un fotón individual en el Photon Map
// -----------------------------------------------------------------------------
struct Photon {
    Vector3 position;   // Posición 3D del fotón en el espacio
    Vector3 directionIncidente;  // Dirección desde la que llegó el fotón (útil para calcular BRDF)
    Pixel flux;       // Energía o color del fotón (Phi), usado para estimar radiancia

    // -------------------------------------------------------------------------
    // Función que devuelve la coordenada del fotón en el eje i (0=x,1=y,2=z)
    // Esto es necesario para el KD-Tree que organiza los fotones por ejes.
    // -------------------------------------------------------------------------
    float positionAt(std::size_t i) const {
        if (i == 0) return position.x; // coordenada X
        if (i == 1) return position.y; // coordenada Y
        return position.z;             // coordenada Z
    }
};

// -----------------------------------------------------------------------------
// Functor que permite al KD-Tree acceder a la posición de un fotón
// Se le pasa un fotón y un índice de eje, y devuelve la coordenada correspondiente
// -----------------------------------------------------------------------------
struct PhotonAxisPosition {
    float operator()(const Photon& p, std::size_t i) const {
        return p.positionAt(i);  // Devuelve la coordenada del fotón para el eje i
    }
};
