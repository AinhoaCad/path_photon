// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include "imagenes.h"

// ---------------------------- Imagen ----------------------------
Imagen::Imagen(string fichero){
    int ret_value;
    m = 1.0;
    FILE *f = fopen(fichero.c_str(), "r");
    if (f == NULL) {
        cerr << "No se pudo abrir el archivo: " << fichero << endl;
        return;
    }
    char cabecera[3];
    ret_value = fscanf(f, "%s", cabecera);
    if (cabecera[0] != 'P' || cabecera[1] != '3') {
        cerr << "Error: Formato de PPM no válido." << endl;
        return;
    }
    char linea[100];
    // Leemos la linea hasta \n
    ret_value = fscanf(f, " %[^\n]s", linea);
    while (linea[0]=='#'){
        if (linea[1]=='M' && linea[2]=='A' && linea[3]=='X' && linea[4]=='='){
            m = stod(&linea[5]);
        }
        ret_value = fscanf(f, "%s", linea);
    }
    ancho = atoi(linea);
    ret_value = fscanf(f, "%d", &alto);
    cout << "Ancho: " << ancho << endl;
    cout << "Alto: " << alto << endl;
    ret_value = fscanf(f, "%d", &c);
    cout << "Resolución: " << c << endl;
    cout << "Leyendo imagen: " << fichero << endl;
    double r, g, b;
    for (int i = 0; i < ancho*alto; i++) {
        ret_value = fscanf(f, "%lf", &r);
        RGB rgb;
        rgb.color[0] = r*m/c;
        ret_value = fscanf(f, "%lf", &g);
        rgb.color[1] = g*m/c;
        ret_value = fscanf(f, "%lf", &b);
        rgb.color[2] = b*m/c;
        imagen.push_back(rgb);
    }
    fclose(f);
    cout << "Imagen leída correctamente." << endl;
}

Imagen::Imagen(int ancho, int alto, int resolucion) {
    this->ancho = ancho;
    this->alto = alto;
    this->m = 1.0;
    this->c = resolucion;
    for (int i = 0; i < ancho*alto; i++) {
        RGB rgb;
        rgb.color[0] = 0.0;
        rgb.color[1] = 0.0;
        rgb.color[2] = 0.0;
        imagen.push_back(rgb);
    }
};

// Modifica el valor de un pixel
void Imagen::cambiarPixel(int x, int y, RGB rgb){
    if (x <= 0 || x > ancho || y <= 0 || y > alto) {
        cerr << "Error: Coordenadas fuera de rango al cambiar pixel." << endl;
        return;
    }
    imagen[(x-1) + (y-1)*ancho].color[0] = rgb.color[0];
    imagen[(x-1) + (y-1)*ancho].color[1] = rgb.color[1];
    imagen[(x-1) + (y-1)*ancho].color[2] = rgb.color[2];
}

// Obtiene el valor rgb de un pixel
RGB Imagen::obtenerPixel(int x, int y){
    if (x <= 0 || x > ancho || y <= 0 || y > alto) {
        cerr << "Error: Coordenadas fuera de rango al obtener pixel." << endl;
        return RGB({0.0,0.0,0.0});
    }
    return imagen[(x-1) + (y-1)*ancho];
}

// Escribe la imagen en un fichero en formato PPM
void Imagen::escribirPPM(string fichero){
    FILE *g = fopen(fichero.c_str(), "w");
    if (g == NULL) {
        cerr << "No se pudo abrir el archivo: " << fichero << endl;
        return;
    }
    fprintf(g, "P3\n");
    fprintf(g, "#MAX=%f\n", m);
    fprintf(g, "%d %d\n", ancho, alto);
    fprintf(g, "%d\n", c);

    int i = 0;
    for (auto it : imagen ) {
        i++;
        fprintf(g, "%d %d %d     ", int(it.color[0]*c/m), int(it.color[1]*c/m), int(it.color[2]*c/m));
        if (i == ancho) {
            i = 0;
            fprintf(g, "\n");
        }
    }
    fclose(g);
}

// Modifica internamente el valor de los pixeles con una curva gamma y un valor de clamping
void Imagen::curvaGammaYclamping(double gamma, double valorClamping) {
    for (RGB& rgb : imagen) {
        if (rgb.color[0] > valorClamping) {
            rgb.color[0] = 1.0;
        } else rgb.color[0] = pow((rgb.color[0]/valorClamping), gamma);
        if (rgb.color[1] > valorClamping) {
            rgb.color[1] = 1.0;
        } else rgb.color[1] = pow((rgb.color[1]/valorClamping), gamma);
        if (rgb.color[2] > valorClamping) {
            rgb.color[2] = 1.0;
        } else rgb.color[2] = pow((rgb.color[2]/valorClamping), gamma);
    }
    m = 1.0;
}
