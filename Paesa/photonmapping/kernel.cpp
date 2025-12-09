// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "kernel.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --------------------------- Kernel ---------------------------
// Evalúa el kernel de Caja, constante para todos los fotones
double KernelCaja::evaluar(double radioFoton, double radioMax) {
    return 1 / (M_PI * radioMax * radioMax);
}

// Evalúa el kernel triangular
double KernelTriangular::evaluar(double radioFoton, double radioMax) {
    return (1 - radioFoton / radioMax) / (M_PI * radioMax * radioMax);
}

// Evalúa el kernel Gaussiano
double KernelGaussiano::evaluar(double radioFoton, double radioMax) {
    return exp(-radioFoton * radioFoton / (2 * sigma * sigma)) / 
        (sqrt(2 * M_PI) * sigma * M_PI * radioMax * radioMax);
}

// Evalúa el kernel de Epanechnikov
double KernelEpanechnikov::evaluar(double radioFoton, double radioMax) {
    return 3 / (4 * M_PI * radioMax * radioMax) * 
        (1 - radioFoton * radioFoton / (radioMax * radioMax));
}

// Evalúa el kernel Quartic
double KernelQuartic::evaluar(double radioFoton, double radioMax) {
    return 15 / (16 * M_PI * radioMax * radioMax) * 
        pow(1 - radioFoton * radioFoton / (radioMax * radioMax), 2);
}

// Evalúa el kernel Tripeso
double KernelTripeso::evaluar(double radioFoton, double radioMax) {
    return 35 / (32 * M_PI * radioMax * radioMax) * 
        pow(1 - radioFoton * radioFoton / (radioMax * radioMax), 3);
}

// Evalúa el kernel Tricubo
double KernelTricubo::evaluar(double radioFoton, double radioMax) {
    return 70 / (81 * M_PI * radioMax * radioMax) * 
        pow(1 - pow(radioFoton / radioMax, 3), 3);
}

// Evalúa el kernel Coseno
double KernelCoseno::evaluar(double radioFoton, double radioMax) {
    return 1 / (4 * radioMax * radioMax) * cos(M_PI * radioFoton / radioMax / 2);
}

// Evalúa el kernel Logístico
double KernelLogistico::evaluar(double radioFoton, double radioMax) {
    return 1 / (2 * radioMax * radioMax) / 
        (exp(radioFoton / radioMax) + 2 + exp(-radioFoton / radioMax));
}
