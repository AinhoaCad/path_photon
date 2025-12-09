// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef ESCENA_H
#define ESCENA_H

#include <list>
#include "coordenadas.h"
#include "geometria.h"
#include "foton.h"
#include "kernel.h"

using namespace std;

// ---------------------------- Escena ----------------------------
class Escena{
    public:
    vector<Geometria*> objetos;
    vector<LuzPuntual*> luces;
    Punto pmin, pmax;

    // Constructor por defecto
    Escena();
    //Constructor con objetos
    Escena(vector<Geometria*> objetos);

    // Funcion para anyadir objetos a la escena
    void anyadir(Geometria* g);
    // Funcion para anyadir luces a la escena
    void anyadirLuz(LuzPuntual* l);

    // Funcion que devuelve la interseccion mas cercana de un rayo con los objetos de la escena
    Interseccion interseccion(Rayo r);

    Caja cajaEnvolvente(); // Bounding box de la escena
    void establecerCajaEnvolvente(Punto p0, Punto p1);  

    // Devuelve el color difuso en un punto de la escena con geometrias difusas
    RGB estimacionSiguienteEventoDifuso(Punto x, Direccion wo, Geometria* geo, Direccion n);
    // Devuelve el color en un punto de la escena con geometrias difusas
    RGB ecuacionRenderDifuso(Punto x, Direccion wo, Geometria* geo, Direccion n, int N);
    // Devuelve la luz directa en un punto de la escena sobre una geometria difusa
    RGB estimacionSiguienteEvento(Punto x, Direccion wo, Geometria* geo, Direccion n, double sigma = 0.0);
    // Devuelve el color en un punto de la escena con geometrias especulares
    RGB ecuacionRender(Punto x, Direccion wo, Geometria* geo, Direccion n);

    // Devuelve un mapa de fotones de la escena
    MapaFotones generarMapaFotones(int numPaseos, bool guardar = false, double sigma = 0.0);
    // Devuelve un mapa de fotones de la escena con las custicas separadas
    void generarMapasFotonesCausticas(int numPaseos, MapaFotones& mapa, MapaFotones& mapaCausticas, bool guardar = false);

    // Devuelve el color en un punto de la escena con geometrias especulares
    // calculando la radiancia en base al mapa de fotones y el kernel
    RGB ecuacionRenderFotones(Punto x, Direccion wo, Geometria* geo, Direccion n, 
        MapaFotones mapa, int kFotones = 20, double radio = 0.1, bool guardar = false,
        Kernel* kernel = new KernelCaja(), double sigma = 0.0);

    // Calcula la radiancia en base a ambos mapas de fotones y el kernel
    RGB ecuacionRenderFotonesProgresivo(Punto x, Direccion wo, Geometria* geo, Direccion n, 
        MapaFotones mapa_base, int emitidos_base, int iteraciones, 
        int emitidos_progresivo, vector<MapaFotones> mapas, double alpha = 0.5, int kFotones = 20, 
        double radio = 0.1, bool guardar = false);
    
    private:
    void primerReboteFoton(Rayo r, RGB luz, list<Foton>& fotones, list<Foton>& causticos, bool guardar, double sigma = 0.0);
    void calcularCaminoFoton(Rayo r, RGB luz, list<Foton>& fotones, list<Foton>& causticos, bool caustico, double sigma = 0.0);
};

// -------------------------- Escenas de prueba --------------------------
// Luz puntual -----------------------------------------------------------
Escena CornellBox(); // Cornell Box base difusa con una luz puntual
Escena CornellBox2(); // Cornell Box base especular con una luz puntual
Escena CornellBoxDosLuces(); // Cornell Box con esfera difusa y dos luces puntuales
Escena CornellBoxColorBleeding(); // Cornell Box con tres esferas difusas y una luz puntual
Escena CornellBoxTexturas(); // Cornell Box con texturas
Escena CornellBoxCausticasPuntual(); // Cornell Box con esfera especular y luz puntual

// Luz area --------------------------------------------------------------
Escena CornellBoxLuzArea(); // Cornell Box base difusa con una luz de area
Escena CornellBoxLuzArea2(); // Cornell Box base especular con una luz de area
Escena CornellBoxSombraSuave(); // Cornell Box con esfera difusa y luz de area
Escena CornellBoxCausticas(); // Cornell Box con esfera especular y luz de area
Escena CornellBoxLuzPeque(); // Cornell Box difusa con luz de area pequeña

// Escenas elaboradas ----------------------------------------------------
Escena OtrasGeometrias(); // Escena con conos, cilindros, caja, disco, esfera y planos
Escena Bonita(); // Escena con un árbol cónico de luz puntual
Escena BonitaLuzArea(); // Escena con un árbol cónico de luces de area
Escena Ajedrez();

#endif // ESCENA_H