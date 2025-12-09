// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "photonmapping.h"
#include "coordenadas.h"
#include "geometria.h"
#include "imagenes.h"
#include "escena.h"
#include "camara.h"
#include "aleatorio.h"
#include "foton.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string CARPETA_SALIDA = "imagenes/";

// --------------------------- PhotonMapper ---------------------------
// Cornell Box --------------------------------------------------------
void pruebasMapaFotones() {
    Escena escena = CornellBox();
    cout << "Escena generada" << endl;

    int numPaseos = 2500;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 5;
    int kFotones = 25;
    double radio = 0.08;
    bool guardar = true;

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, guardar);

    cout << "Mapa de fotones generado" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara.generarImagenFotones(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, guardar);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapaFotones.ppm");
    cout << "Imagen generada" << endl;
}

void pruebasMapaFotonesMulti() {
    Escena escena = CornellBox();
    cout << "Escena generada" << endl;

    int numPaseos = 10000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 5;
    int kFotones = 20;
    double radio = 0.16;
    bool guardar = true;
    int numThreads = 10;

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, guardar);

    cout << "Mapa de fotones generado" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara./* The above code appears to be a comment section in C++ programming
    language. It mentions a function or task called
    "generarImagenFotonesParalela" which seems to be related to generating an
    image of photons in a parallel manner. However, since it is commented
    out, it is not actively being executed or compiled as part of the
    program. */
    generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, guardar, numThreads);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapaFotonesP.ppm");
    cout << "Imagen generada" << endl;
}

void pruebasFotonesReflejos() {
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;

    int numPaseos = 25000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 10;
    int kFotones = 25;
    double radio = 0.05;
    bool guardar = true;

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, guardar);

    cout << "Mapa de fotones generado" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara.generarImagenFotones(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, guardar);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapaFotones.ppm");
    cout << "Imagen generada" << endl;  
}

void pruebasFotonesReflejosMulti() {
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;

    int numPaseos = 2000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 25;
    int kFotones = 35;
    double radio = 0.25;
    bool guardar = false;
    int numThreads = 12;
    double sigma = 2.5;
    Kernel* kernel = new KernelGaussiano();

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, guardar, sigma);

    cout << "Mapa de fotones generado" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, guardar, numThreads, kernel, sigma);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapaFotonesP.ppm");
    cout << "Imagen generada" << endl;
}

// Memorias -----------------------------------------------------------
void pruebasDirectaFotones() {
    Escena escena = CornellBox();
    cout << "Escena generada" << endl;
    int numPaseos = 80000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 20;
    int kFotones = 250;
    double radio = 0.2;
    int numThreads = 10;

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, true);
    cout << "Mapa de fotones generado" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));
    Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/SoloPrimerRebote.ppm");
    cout << "Imagen generada" << endl;

    mapaFotones = escena.generarMapaFotones(numPaseos, false);
    cout << "Mapa de fotones generado" << endl;
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, false, numThreads);
    cout << "Generando imagen" << endl;

    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/SoloNextEvent.ppm");
    cout << "Imagen generada" << endl;
}

void pruebasParametros(){
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    int numPaseos = 100000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 10;
    int kFotones = 10;
    double radio = 0.1;
    int numThreads = 10;
    
    // Bucle Num fotones
    for (int i = 1000; i <= numPaseos; i*=10) {

        clock_t start = clock();
        MapaFotones mapaFotones = escena.generarMapaFotones(i, true);
        clock_t end = clock();
        cout << "Mapa de fotones generado" << endl;
        cout << "Tiempo segundos mapa: " << double(end - start) / CLOCKS_PER_SEC << endl;

        start = clock();
        Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
            rayos, mapaFotones, kFotones, radio, true, numThreads);
        end = clock();
        cout << "Generando imagen" << endl;
        cout << "Tiempo segundos imagen con " << numThreads << " hilos y " << i << " Num fotones: "  << double(end - start) / CLOCKS_PER_SEC << endl;

        imagen.curvaGamma(1/2.2);
        imagen.escribirPPM("imagenes/parametrosNfotones" + to_string(i) + ".ppm");
        cout << "Imagen generada" << endl;
    }

    // Bucle cercanos
    for(int i = 1; i<=100; i*=10){
        clock_t start = clock();
        MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, true);
        clock_t end = clock();
        cout << "Mapa de fotones generado" << endl;
        cout << "Tiempo segundos mapa: " << double(end - start) / CLOCKS_PER_SEC << endl;

        start = clock();
        Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
            rayos, mapaFotones, i, radio, true, numThreads);
        end = clock();
        cout << "Generando imagen" << endl;
        cout << "Tiempo segundos imagen con " << numThreads << " hilos y " << i << " vecinos: " << double(end - start) / CLOCKS_PER_SEC << endl;

        imagen.curvaGamma(1/2.2);
        imagen.escribirPPM("imagenes/parametrosKfotones" + to_string(i) + ".ppm");
        cout << "Imagen generada" << endl;
    }
}

void pruebasKernels() {
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    int numPaseos = 10000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 10;
    int kFotones = 10;
    double radio = 0.1;
    int numThreads = 10;

    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, true);
    cout << "Mapa de fotones generado" << endl;
    Kernel* kernel = new KernelCaja();
    Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel caja" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-caja.ppm");

    kernel = new KernelTriangular();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel triangular" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-triangular.ppm");

    kernel = new KernelGaussiano();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel gaussiano" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-gaussiano.ppm");

    kernel = new KernelEpanechnikov();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel epanechnikov" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-epanechnikov.ppm");

    kernel = new KernelQuartic();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel quartic" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-quartic.ppm");

    kernel = new KernelTripeso();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel tripeso" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-tripeso.ppm");

    kernel = new KernelTricubo();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel tricubo" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-tricubo.ppm");

    kernel = new KernelCoseno();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel coseno" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-coseno.ppm");

    kernel = new KernelLogistico();
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, true, numThreads, kernel);
    cout << "Generando imagen con kernel logistico" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/kernel-logistico.ppm");

    cout << "Imagenes generadas" << endl;
}

void pruebasMapaCaustico() {
    //Escena escena = CornellBox2();
    Escena escena = CornellBoxCausticasPuntual();
    cout << "Escena generada" << endl;
    Kernel* kernel = new KernelEpanechnikov();

    int numPaseos = 5000;
    int altura = 512;
    int anchura = 512;
    int resolucion = 256;
    int rayos = 15;
    int kFotones = 40;
    int kCaustico = 25;
    double radio = 0.25;
    double radioCaustico = 0.15;
    bool guardar = false;
    int numThreads = 10;

    MapaFotones mapaFotones;
    MapaFotones mapaCaustico;
    escena.generarMapasFotonesCausticas(numPaseos, mapaFotones, mapaCaustico, guardar);
    MapaFotones mapa = escena.generarMapaFotones(numPaseos, guardar);

    cout << "Mapas de fotones generados" << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapa, kFotones, radio, guardar, numThreads, kernel);
    cout << "Generando imagen base" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasBase.ppm");


    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaCaustico, kCaustico, radioCaustico, true, numThreads, kernel);
    cout << "Generando imagen solo causticas" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasSoloCausticas.ppm");

    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, guardar, numThreads, kernel);
    cout << "Generando imagen solo difusos" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasSoloDifusos.ppm");

    imagen = camara.generarImagenFotonesParalelaMapaCaustico(escena, anchura, altura, resolucion, 
        rayos, mapaFotones, kFotones, radio, mapaCaustico, kCaustico, radioCaustico, guardar, numThreads, kernel);
    cout << "Generando imagen con causticas" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasPonderadoCausticas.ppm");

    cout << "Imagenes generadas" << endl;  
}

void pruebasMapaProgresivo() {
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;

    int numPaseos_base = 1000;
    int numPaseos_progresivo = 500;
    int iteraciones = 5;
    int num_alpha = 2;
    int altura = 512, anchura = 512, resolucion = 256;
    int rayos = 20;
    int kFotones = 35;
    double radio = 0.35;
    bool guardar = true;
    int numThreads = 10;

    MapaFotones mapa_base;
    MapaFotones mapaCausticas;
    escena.generarMapasFotonesCausticas(numPaseos_base, mapa_base, mapaCausticas, guardar);

    cout << "Mapas de fotones generados" << endl;
    cout << "Mapa base: " << mapa_base << endl;
    CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

    Imagen imagen = camara.generarImagenFotonesParalelaMapaCaustico(escena, anchura, altura, resolucion, 
        rayos, mapa_base, kFotones, radio, mapaCausticas, kFotones, radio, guardar, numThreads);
    cout << "Generando imagen solo base" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasSoloBase.ppm");

    for (int i = 0; i <= num_alpha; i++) {
        imagen = camara.generarImagenFotonesParalelaProgresivo(escena, anchura, altura, resolucion, 
            rayos, mapaCausticas, mapa_base, numPaseos_base, iteraciones, numPaseos_progresivo, (double)i/num_alpha,
            kFotones, radio, guardar, numThreads);
        cout << "Generando imagen con progresivo " << (double)i/num_alpha << endl;
        imagen.curvaGamma(1/2.2);
        imagen.escribirPPM("imagenes/mapasPonderadoProgresivo-" + to_string((double)i/num_alpha) + ".ppm");
    }

    MapaFotones mapa = escena.generarMapaFotones(numPaseos_base + iteraciones*numPaseos_progresivo, guardar);
    imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        rayos, mapa, kFotones, radio, guardar, numThreads);
    cout << "Generando imagen con mapa normal" << endl;
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/mapasProgresivoComparar.ppm");

    cout << "Imagenes generadas" << endl;  
}

void pruebasMedios() {
    Escena escena = CornellBox2();
    cout << "Escena generada" << endl;

    int numPaseos = 2000;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    int rayos = 25;
    int kFotones = 35;
    double radio = 0.25;
    bool guardar = false;
    int numThreads = 12;
    double sigmaStart = 1.0;
    double sigmaSteo = 1.0;
    double sigmaEnd = 3.0;
    Kernel* kernel = new KernelGaussiano();

    for (double sigma = sigmaStart; sigma <= sigmaEnd; sigma += sigmaSteo) {
        MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, guardar, sigma);
        cout << "Mapa de fotones generado" << endl;
        CamaraEstenopeica camara = CamaraEstenopeica(Punto(0,0,-3.5), Direccion(-1,0,0), Direccion(0,1,0), Direccion(0,0,3));

        Imagen imagen = camara.generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
            rayos, mapaFotones, kFotones, radio, guardar, numThreads, kernel, sigma);
        cout << "Generando imagen con sigma " << sigma << endl;

        imagen.curvaGamma(1/2.2);
        imagen.escribirPPM("imagenes/mapaFotonesSigma" + to_string(sigma) + ".ppm");
        cout << "Imagen generada" << endl;
    }
}

// Main ---------------------------------------------------------------

// Función auxiliar
void parseEscena(int id, Escena& e, Punto& p, Direccion& d, Direccion& u, Direccion& f) {
    switch (id) {
        case 1:
            e = CornellBox2();
            break;
        case 2:
            e = CornellBoxDosLuces();
            break;
        case 3:
            e = CornellBoxColorBleeding();
            break;
        case 4:
            e = CornellBoxTexturas();
            break;
        case 5:
            e = CornellBoxCausticasPuntual();
            break;
        case 6:
            e = OtrasGeometrias();
            break;
        case 7:
            d = Direccion(-2,0,0);
            u = Direccion(0,2,0);
            f = Direccion(0,0,2);
            e = Bonita();
            break;
        case 8:
            p = Punto(0,0,-1);
            d = Direccion(-0.5,0,0);
            u = Direccion(0,0.5,0);
            f = Direccion(0,0,0.5);
            e = Ajedrez();
            break;
        default:
            e = CornellBox();
            break;
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));
    // El parametro -s indica la escena
    // -r <numRayos> indica el numero de rayos por pixel
    // -p <numPaseos> indica el numero de paseos de los fotones
    // -R <radio> indica el radio de busqueda de los fotones
    // -k <kVecinos> indica el numero de fotones a usar
    // -a <apertura> indica la apertura de la camara
    // -t <numThreads> indica el numero de hilos a usar
    // -N indica la altura de la imagen
    // -M indica la anchura de la imagen
    // -c indica la resolucion de color
    // -K indica el kernel a usar
    string usage = "Uso: ./pruebas -s <escena> [-r <numRayos>] [-p <numPaseos>] [-R <radio>] [-k <kVecinos>]\n\t\t[-a <apertura>] [-t <numThreads>] [-N <altura>] [-M <anchura>] [-c <resolucionColor>]\n\t\t[-K <kernel>]";

    // Parsear argumentos
    if (argc < 2) {
        cout << usage << endl;
        return 0;
    }

    Escena escena = CornellBox();
    int numRayos = 5;
    int numPaseos = 1000;
    double radio = 0.25;
    int kVecinos = 100;
    double apertura = -1;
    int numThreads = 1;
    int altura = 256;
    int anchura = 256;
    int resolucion = 256;
    Kernel* kernel = new KernelCaja();
    Punto p = Punto(0,0,-3.5);
    Direccion d = Direccion(-1,0,0);
    Direccion u = Direccion(0,1,0);
    Direccion f = Direccion(0,0,3);

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-s") {
            if (i + 1 < argc) parseEscena(atoi(argv[i+1]), escena, p, d, u, f);
        } else if (string(argv[i]) == "-r") {
            if (i + 1 < argc) numRayos = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-p") {
            if (i + 1 < argc) numPaseos = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-R") {
            if (i + 1 < argc) radio = atof(argv[i+1]);
        } else if (string(argv[i]) == "-k") {
            if (i + 1 < argc) kVecinos = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-a") {
            if (i + 1 < argc) apertura = atof(argv[i+1]);
        } else if (string(argv[i]) == "-t") {
            if (i + 1 < argc) numThreads = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-N") {
            if (i + 1 < argc) altura = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-M") {
            if (i + 1 < argc) anchura = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-c") {
            if (i + 1 < argc) resolucion = atoi(argv[i+1]);
        } else if (string(argv[i]) == "-K") {
            if (i + 1 < argc) {
                if (string(argv[i+1]) == "triangular") {
                    kernel = new KernelTriangular();
                } else if (string(argv[i+1]) == "gaussiano") {
                    kernel = new KernelGaussiano();
                } else if (string(argv[i+1]) == "epanechnikov") {
                    kernel = new KernelEpanechnikov();
                } else if (string(argv[i+1]) == "quartic") {
                    kernel = new KernelQuartic();
                } else if (string(argv[i+1]) == "tripeso") {
                    kernel = new KernelTripeso();
                } else if (string(argv[i+1]) == "tricubo") {
                    kernel = new KernelTricubo();
                } else if (string(argv[i+1]) == "coseno") {
                    kernel = new KernelCoseno();
                } else if (string(argv[i+1]) == "logistico") {
                    kernel = new KernelLogistico();
                }
            }
        }
        i++;
    }

    Camara* camara = new CamaraEstenopeica(p, d, u, f);
    if (apertura > 0) {
        camara = new CamaraLente(p, d, u, f, apertura);
    }

    cout << "Generando imagen con photon mapping" << endl;
    MapaFotones mapaFotones = escena.generarMapaFotones(numPaseos, true);
    Imagen imagen = camara->generarImagenFotonesParalela(escena, anchura, altura, resolucion, 
        numRayos, mapaFotones, kVecinos, radio, false, numThreads, kernel);
    imagen.curvaGamma(1/2.2);
    imagen.escribirPPM("imagenes/photonMapping.ppm");
    cout << "Imagen generada" << endl;

    return 0;
}
