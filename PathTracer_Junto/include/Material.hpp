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

    // DE MOMENTO SACAMOS EL COLOR DEL MATERIAL COMO EL COLOR DIFUSO PERO EN EL FUTURO SE PODRÍA HACER MÁS COMPLEJO
    // USANDO POR EJEMPLO MEZCLA DE DIFUSO Y ESPECULAR 
    Pixel toPixel() const {
        return kd.toPixel();  // Usa kd como color principal del material
    }
};



// Declaraciones de materiales predeterminados
extern Material whiteDiffuse;
extern Material redDiffuse;
extern Material greenDiffuse;
extern Material blueDiffuse;
extern Material purpleDiffuse;
extern Material grayDiffuse;
extern Material mirrorSpecular;
extern Material glassDielectric;
