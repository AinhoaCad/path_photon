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
            throw("Conservación de energía violada en el material.");
        }
    }

    // Obtener el color del material para visualización
    Pixel toPixel() const {
        // Combinar componentes difusa y especular para obtener un color representativo
        Color combinedColor;
        combinedColor.R = kd.R + ks.R * 0.3f; // Reducir contribución especular
        combinedColor.G = kd.G + ks.G * 0.3f;
        combinedColor.B = kd.B + ks.B * 0.3f;
        
        // Para materiales dieléctricos, añadir un tinte
        if (kt.R > 0.1f || kt.G > 0.1f || kt.B > 0.1f) {
            combinedColor.R = std::max(combinedColor.R, 0.2f);
            combinedColor.G = std::max(combinedColor.G, 0.2f);
            combinedColor.B = std::max(combinedColor.B, 0.3f); // Ligero tinte azul
        }
        
        // Asegurar que no exceda 1.0
        combinedColor.R = std::min(combinedColor.R, 1.0f);
        combinedColor.G = std::min(combinedColor.G, 1.0f);
        combinedColor.B = std::min(combinedColor.B, 1.0f);
        
        return combinedColor.toPixel();
    }
    
    // Métodos para identificar tipo de material
    bool isDiffuse() const {
        return (kd.R > 0.01f || kd.G > 0.01f || kd.B > 0.01f) && 
               (ks.R < 0.1f && ks.G < 0.1f && ks.B < 0.1f) &&
               (kt.R < 0.1f && kt.G < 0.1f && kt.B < 0.1f);
    }
    
    bool isSpecular() const {
        return (ks.R > 0.1f || ks.G > 0.1f || ks.B > 0.1f);
    }
    
    bool isDielectric() const {
        return (kt.R > 0.1f || kt.G > 0.1f || kt.B > 0.1f);
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
