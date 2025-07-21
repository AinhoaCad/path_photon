#pragma once

#include "Color.hpp"

class Material {
public:
    Color kd;  // Reflectancia difusa
    Color ks;  // Reflectancia especular
    Color kt;  // Coeficiente de transmisión/refracción
    Color Le;  // Emisión (color de luz emitida)
    float ior; // Índice de refracción (para materiales dieléctricos)

    // Constructores
    Material()
        : kd(0, 0, 0), ks(0, 0, 0), kt(0, 0, 0), Le(0, 0, 0), ior(1.0f) {}
    Material(const Color& kd, const Color& ks, const Color& kt, const Color& Le, float ior)
        : kd(kd), ks(ks), kt(kt), Le(Le), ior(ior) {}

    // Validar conservación de energía
    void validateEnergyConservation() const {
        if (kd.R + ks.R + kt.R > 1.0f || kd.G + ks.G + kt.G > 1.0f || kd.B + ks.B + kt.B > 1.0f) {
            throw("Conservación de energía violada en el material.");
        }
    }
};

// Declaraciones de materiales predeterminados
extern Material Diffuse_White;
extern Material Diffuse_Red;
extern Material Specular_Mirror;
extern Material Dielectric_Glass;
