// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef FOTON_H
#define FOTON_H

#include <list>
#include "rgb.h"
#include "coordenadas.h"
#include "kdtree.h"

// ---------------------------- Foton ----------------------------
class Foton {
    public:
    Punto posicion;
    Direccion direccion;
    RGB flujo;

    Foton(Punto p, Direccion d, RGB f) : posicion(p), direccion(d), flujo(f) {}
    ~Foton() {}

    // Devuelve la coordenada i de la posicion del foton, necesaria para el KDTree
    double position(int i) const {
        return posicion.h[i];
    }
};

ostream& operator<<(ostream& os, const Foton& f);

// Struct necesario para el KDTree
struct PosicionEjeFoton {
    double operator()(const Foton& f, int i) const {
        return f.position(i);
    }
};

// Mapa de fotones redefinido por comodidad
using MapaFotones = nn::KDTree<Foton,3,PosicionEjeFoton>;
MapaFotones constructorMapaFotones(std::list<Foton> fotones);

ostream& operator<<(ostream& os, const MapaFotones& m);

#endif // FOTON_H