// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef CAMARA_H
#define CAMARA_H

#include "coordenadas.h"
#include "imagenes.h"
#include "escena.h"
#include "foton.h"
#include "kernel.h"

// Clase genérica para generar una camara
class Camara{
    public:
    Punto O;            // Posicion de la camara
    Direccion L, U, F;  // Vectores de la camara
    
    // Constructor
    Camara(Punto o, Direccion l, Direccion u, Direccion f) : O(o), L(l), U(u), F(f) {};

    // Genera un rayo aleatorio en el pixel (i,j) a partir de la camara
    virtual Rayo generarRayo(int i, int j, int ancho, int alto) = 0;

    // Genera una imagen con las normales de las superficies de la escena
    Imagen generarImagenNormales(Escena e, int ancho, int alto, int resolucion, int rayos);

    // Genera una imagen con los colores de las superficies de la escena
    Imagen generarImagenColores(Escena e, int ancho, int alto, int resolucion, int rayos);

    // Genera una imagen solo con la luz directa a partir de la escena
    Imagen generarImagenDirecta(Escena e, int ancho, int alto, int resolucion, int rayos);

    // Genera una imagen con la luz directa y la luz indirecta con superficies difusas de la escena
    Imagen generarImagenDifuso(Escena e, int ancho, int alto, int resolucion, int rayos, int N);

    // Genera una imagen completa (path Tracer) a partir de la escena
    Imagen generarImagen(Escena e, int ancho, int alto, int resolucion, int rayos);

    // Genera una imagen completa (path Tracer) a partir de la escena con paralelizacion
    Imagen generarImagenParalela(Escena e, int ancho, int alto, int resolucion, int rayos, int numThreads = 4);

    // Genera una imagen completa (photon Mapper) a partir de la escena
    Imagen generarImagenFotones(Escena e, int ancho, int alto, int resolucion, int rayos, 
        MapaFotones mapa, int kFotones = 20, double radio = 0.1, bool guardar = false, Kernel* kernel = new KernelCaja());
    
    // Genera una imagen completa (photon Mapper) con causticas separadas
    Imagen generarImagenMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
        MapaFotones mapa, int kFotones, double radio, 
        MapaFotones mapaCausticas, int kFotonesCausticas = 20, double radioCausticas = 0.1,
        bool guardar = false, Kernel* kernel = new KernelCaja());

    // Genera una imagen completa (photon Mapper) a partir de la escena con paralelizacion
    Imagen generarImagenFotonesParalela(Escena e, int ancho, int alto, int resolucion, int rayos, 
        MapaFotones mapa, int kFotones = 20, double radio = 0.1, bool guardar = false, int numThreads = 4, 
        Kernel* kernel = new KernelCaja(), double sigma = 0.0);

    // Genera una imagen completa (photon Mapper) con causticas separadas
    Imagen generarImagenFotonesParalelaMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
        MapaFotones mapa, int kFotones, double radio, 
        MapaFotones mapaCausticas, int kFotonesCausticas = 20, double radioCausticas = 0.1,
        bool guardar = false, int numThreads = 4, Kernel* kernel = new KernelCaja());    

    // Genera una imagen completa a partir de dos mapas de fotones (progresivo)
    Imagen generarImagenFotonesParalelaProgresivo(Escena e, int ancho, int alto, int resolucion, int rayos, 
        MapaFotones mapaCausticas, MapaFotones mapa_base, int emitidos_base, int iteraciones, 
        int emitidos_progresivo, double alpha = 0.5, int kFotones = 20, 
        double radio = 0.1, bool guardar = false, int numThreads = 4);

    private:
    // Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
    void generarImagenParalelaAux(Escena e, int ancho, int alto, int resolucion, int rayos, Imagen* imagen, int numThreads = 4, int hilo=0);
    
    // Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
    void generarImagenFotonesParalelaAux(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapa, int kFotones = 20, double radio = 0.1, 
            bool guardar = false, int numThreads = 4, int hilo=0, Kernel* kernel = new KernelCaja(), double sigma = 0.0);

    // Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
    void generarImagenFotonesParalelaAuxMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapa, int kFotones, double radio,
            MapaFotones mapaCausticas, int kFotonesCausticas = 20, double radioCausticas = 0.1, 
            bool guardar = false, int numThreads = 4, int hilo=0, Kernel* kernel = new KernelCaja());

    // Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
    void generarImagenFotonesParalelaAuxProgresivo(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapaCausticas, MapaFotones mapa_base, int emitidos_base, int iteraciones, 
            int emitidos_progresivo, vector<MapaFotones> mapas, double alpha = 0.5, int kFotones = 20, 
            double radio = 0.1, bool guardar = false, int numThreads = 4, int hilo=0);
};

// Clase para generar una camara ortogonal
class CamaraEstenopeica : public Camara{
    public:
    // Constructor
    CamaraEstenopeica(Punto o, Direccion l, Direccion u, Direccion f) : Camara(o, l, u, f) {};

    // Genera un rayo aleatorio en el pixel (i,j) a partir de la camara
    Rayo generarRayo(int i, int j, int ancho, int alto);
};

// Clase para generar una camara con lente
class CamaraLente : public Camara{
    public:
    double radio_apertura;
    // Constructor
    CamaraLente(Punto o, Direccion l, Direccion u, Direccion f, double r) : Camara(o, l, u, f), radio_apertura(r) {};

    // Genera un rayo aleatorio en el pixel (i,j) a partir de la camara
    Rayo generarRayo(int i, int j, int ancho, int alto);
};

#endif // CAMARA_H