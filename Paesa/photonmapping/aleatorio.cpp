// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "aleatorio.h"
#include <cmath>
#include <random>

using namespace std;

// Matriz de cambio de base
MatrizT DireccionMuestraAleatoria::cambioBase() {
    Direccion aleatorio = Direccion((double)rand() / RAND_MAX,
        (double)rand() / RAND_MAX, (double)rand() / RAND_MAX).normalizado();
    Direccion y = (normal).productoVectorial(aleatorio).normalizado();
    while (y.modulo() == 0) {
        aleatorio = Direccion((double)rand() / RAND_MAX,
        (double)rand() / RAND_MAX, (double)rand() / RAND_MAX).normalizado();
        y = (normal).productoVectorial(aleatorio).normalizado();
    }
    Direccion x = y.productoVectorial(normal).normalizado();
    return MatrizT(x,y,normal.normalizado(), this->x);
}

// Genera una direccion aleatoria en la hemiesfera
Direccion DireccionMuestraAleatoria::anguloUniforme() {
    double xi1 = (double)rand() / RAND_MAX;
    double th = (M_PI/2)*xi1;
    double xi2 = (double)rand() / RAND_MAX;
    double phi = 2*M_PI*xi2;
    Direccion d = Direccion(sin(th)*cos(phi), sin(th)*sin(phi), cos(th));
    return cambioBase()*d;
}

// Genera una direccion aleatoria en la hemiesfera con angulo solido uniforme
Direccion DireccionMuestraAleatoria::anguloSolidoUniforme() {
    double xi1 = (double)rand() / RAND_MAX;
    double th = acos(xi1);
    double xi2 = (double)rand() / RAND_MAX;
    double phi = 2*M_PI*xi2;
    Direccion d = Direccion(sin(th)*cos(phi), sin(th)*sin(phi), cos(th));
    return cambioBase()*d;
}

// Genera una direccion aleatoria en la hemiesfera con coseno uniforme
Direccion DireccionMuestraAleatoria::cosenoUniforme() {
    double xi1 = (double)rand() / RAND_MAX;
    double th = acos(sqrt(1-xi1));
    double xi2 = (double)rand() / RAND_MAX;
    double phi = 2*M_PI*xi2;
    Direccion d = Direccion(sin(th)*cos(phi), sin(th)*sin(phi), cos(th));
    return cambioBase()*d;
}

// Genera una direccion aleatoria en la esfera con angulo solido uniforme
Direccion muestraAnguloSolido() {
    double xi1 = (double)rand() / RAND_MAX;
    double th = acos(2*xi1-1);
    double xi2 = (double)rand() / RAND_MAX;
    double phi = 2*M_PI*xi2;
    Direccion d = Direccion(sin(th)*cos(phi), sin(th)*sin(phi), cos(th));
    return d;
}