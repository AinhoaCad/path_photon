// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef IMAGENES_H
#define IMAGENES_H
#include "rgb.h"
#include <vector>

using namespace std;

// ---------------------------- Imagen ----------------------------
class Imagen {
    public:
    int c; // resolucion de color
    double m; // valor maximo de la imagen
    int ancho, alto; // dimensiones de la imagen
    vector<RGB> imagen; // vector de pixeles

    // Constructor a partir de un fichero en formato PPM
    Imagen(string fichero);
    // Constructor a partir de un ancho y un alto
    Imagen(int ancho, int alto, int resolucion);

    // Modifica el valor de un pixel
    void cambiarPixel(int x, int y, RGB rgb);
    // Obtiene el valor rgb de un pixel
    RGB obtenerPixel(int x, int y);

    // Escribe la imagen en un fichero en formato PPM
    void escribirPPM(string fichero);

    // Modifica internamente el valor de los pixeles para que el valor maximo sea 1
    void clamping() {curvaGammaYclamping(1.0, 1.0);};
    // Modifica internamente el valor de los pixeles para que el color se reescale
    void ecualizar() {curvaGammaYclamping(1.0, m);};
    void ecualizarYclamping(double valorClamping) {curvaGammaYclamping(1.0, valorClamping);};
    // Modifica internamente el valor de los pixeles con una curva gamma
    void curvaGamma(double gamma) {curvaGammaYclamping(gamma, m);};
    void curvaGammaYclamping(double gamma, double valorClamping);
};

#endif