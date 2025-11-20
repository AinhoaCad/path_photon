#pragma once

#include "Color.hpp"
#include <algorithm>

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
            throw("Conservación de energía no se cumple en el material.");
        }
    }

    // Obtener el color del material para visualización
    Pixel toPixel() const {
        // Mostrar la emisión si está definida (Le != 0).
        // Según la especificación, la propiedad "color" previa se interpreta
        // ahora como emisión (Le). Para no perder visibilidad durante la
        // transición, si Le es nulo devolvemos kd como color de depuración.
        if (Le.R > 0.0f || Le.G > 0.0f || Le.B > 0.0f) {
            return Le.toPixel();
        }
        // En caso contrario, mostrar el color difuso por defecto
        return kd.toPixel();
    }
    
    // Métodos para identificar tipo de material
    bool isDiffuse() const {
        return (kd.R > 0.0f || kd.G > 0.0f || kd.B > 0.0f);
    }
    
    bool isSpecular() const {
        return (ks.R > 0.0f || ks.G > 0.0f || ks.B > 0.0f);
    }
    
    bool isDielectric() const {
        return (kt.R > 0.0f || kt.G > 0.0f || kt.B > 0.0f);
    }

    bool isEspejoPuro() const {
    return isSpecular() && !isDiffuse() && !isDielectric();
}

    bool isPlastic() const {
        return isDiffuse() && isSpecular() && !isDielectric();
    }
};



// Declaraciones de materiales predeterminados
extern Material whiteDiffuse;
extern Material redDiffuse;
extern Material greenDiffuse;
extern Material blueDiffuse;
extern Material purpleDiffuse;
extern Material cyanDiffuse;
extern Material grayDiffuse;
extern Material mirrorSpecular;
extern Material glassDielectric;
extern Material plasticMaterial;
extern Material shinyGold;
extern Material glossyPlasticStrong;
extern Material coloredGlassStrong;


//Para las luces neon emisivo
extern Material neonEmissive;
