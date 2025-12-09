// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef RGB_H
#define RGB_H

#include <iostream>
#include <sstream>

using namespace std;

// ---------------------------- RGB ----------------------------
struct RGB {
    public:
    double color[3];

    inline RGB() : color{0,0,0} {};

    inline RGB(double r, double g, double b) : color{r,g,b} {};

    inline RGB operator+(const RGB& rhs) const {
        return RGB(color[0] + rhs.color[0], color[1] + rhs.color[1], color[2] + rhs.color[2]);
    }

    inline RGB operator-(const RGB& rhs) const {
        return RGB(color[0] - rhs.color[0], color[1] - rhs.color[1], color[2] - rhs.color[2]);
    }

    inline RGB operator*(const RGB& rhs) const {
        return RGB(color[0] * rhs.color[0], color[1] * rhs.color[1], color[2] * rhs.color[2]);
    }

    inline RGB operator*(const double& rhs) const {
        return RGB(color[0] * rhs, color[1] * rhs, color[2] * rhs);
    }

    inline RGB operator/(const double& rhs) const {
        return RGB(color[0] / rhs, color[1] / rhs, color[2] / rhs);
    }

};

inline RGB operator*(const double& lhs, const RGB& rhs) {
    return RGB(lhs * rhs.color[0], lhs * rhs.color[1], lhs * rhs.color[2]);
};

inline ostream& operator<<(ostream& os, const RGB& rgb) {
    os << "RGB(" << rgb.color[0] << ", " << rgb.color[1] << ", " << rgb.color[2] << ")";
    return os;
};

inline double max(const RGB& rgb) {
    return max(rgb.color[0], max(rgb.color[1], rgb.color[2]));
};

// Colores predefinidos
const RGB RGB_ROJO = RGB(1,0,0);
const RGB RGB_VERDE = RGB(0,1,0);
const RGB RGB_AZUL = RGB(0,0,1);
const RGB RGB_BLANCO = RGB(1,1,1);
const RGB RGB_GRIS_CLARO = RGB({0.75,0.75,0.75});
const RGB RGB_GRIS = RGB({0.5,0.5,0.5});
const RGB RGB_NEGRO = RGB(0,0,0);
const RGB RGB_AMARILLO = RGB(1,1,0);
const RGB RGB_MORADO = RGB({0.5,0.0,0.5});
const RGB RGB_MAGENTA = RGB({1,0.0,1});
const RGB RGB_ROSA = RGB({1.0,0.5,1.0});
const RGB RGB_CIAN = RGB({0.5,1.0,1.0});
const RGB RGB_CIAN2 = RGB({0,1.0,1.0});

#endif // RGB_H