// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef ALEATORIO_H
#define ALEATORIO_H

#include "coordenadas.h"
#include <time.h>

// Clase para generar numeros aleatorios
class DireccionMuestraAleatoria {
    public:
    Direccion normal;   // Normal de la superficie
    Punto x;            // Punto de la superficie

    // Constructor
    DireccionMuestraAleatoria(Direccion normal, Punto x) : normal(normal), x(x) {
        //srand(time(NULL));
    };

    // Matriz de cambio de base
    MatrizT cambioBase();

    // Genera una direccion aleatoria en la hemiesfera
    Direccion anguloUniforme();

    // Genera una direccion aleatoria en la hemiesfera con angulo solido uniforme
    Direccion anguloSolidoUniforme();

    // Genera una direccion aleatoria en la hemiesfera con coseno uniforme
    Direccion cosenoUniforme();
};

// Genera una direccion aleatoria en la esfera con angulo solido uniforme
Direccion muestraAnguloSolido();

#endif // ALEATORIO_H