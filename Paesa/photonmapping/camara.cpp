// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "camara.h"
#include <thread>
#include <mutex>

// Genera un rayo aleatorio en el pixel (i,j) a partir de la camara
Rayo CamaraEstenopeica::generarRayo(int i, int j, int ancho, int alto) {
    if (i <= 0 || i > ancho || j <= 0 || j > alto) {
        cout << "Error: rayo fuera de la imagen en la camara estenopeica" << endl;
        return Rayo(O, F);
    } else {
        double xrand = (double)rand() / RAND_MAX;
        double yrand = (double)rand() / RAND_MAX;
        double x = 1-(double)2*(i+xrand-1)/(ancho-1);
        double y = 1-(double)2*(j+yrand-1)/(alto-1);
        Direccion d = (F + x*L + y*U).normalizado();
        return Rayo(O, d);
    }
}

// Genera un rayo aleatorio en el pixel (i,j) a partir de la camara
Rayo CamaraLente::generarRayo(int i, int j, int ancho, int alto) {
    if (i <= 0 || i > ancho || j <= 0 || j > alto) {
        cout << "Error: rayo fuera de la imagen en la camara lente" << endl;
        return Rayo(O, F);
    } else {
        double r_rand = radio_apertura * (double)rand() / RAND_MAX;
        double theta_rand = 2*M_PI * (double)rand() / RAND_MAX;
        double u_lente = r_rand * cos(theta_rand);
        double v_lente = r_rand * sin(theta_rand);

        double xrand = (double)rand() / RAND_MAX;
        double yrand = (double)rand() / RAND_MAX;
        double x = 1-(double)2*(i+xrand-1)/(ancho-1);
        double y = 1-(double)2*(j+yrand-1)/(alto-1);

        Punto origen_rayo = O + u_lente/2*L.normalizado() + v_lente/2*U.normalizado();
        Punto centro = O + F + x*L + y*U;
        return Rayo(origen_rayo, (centro-origen_rayo).normalizado());
    }
}

// -----------------------------------------------------------------------------
// Genera una imagen con las normales de las superficies de la escena
Imagen Camara::generarImagenNormales(Escena e, int ancho, int alto, int resolucion, int rayos) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.5,0.5,0.5});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0) {
                    Direccion n = interseccion.normal*0.5;
                    if (r.d*n > 0.0)
                        n = -n;
                    rgb = rgb + RGB({n.h[2],n.h[1],n.h[0]})/rayos;
                }
            }
            imagen.cambiarPixel(i,j,rgb);
        }
    }
    return imagen;
}

// Genera una imagen con los colores de las superficies de la escena
Imagen Camara::generarImagenColores(Escena e, int ancho, int alto, int resolucion, int rayos) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0) {
                    rgb = rgb + interseccion.geometria->k_d/rayos;
                }
            }
            imagen.cambiarPixel(i,j,rgb);
        }
    }
    return imagen;
}

// Genera una imagen solo con la luz directa a partir de la escena
Imagen Camara::generarImagenDirecta(Escena e, int ancho, int alto, int resolucion, int rayos) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0) {
                    RGB c = e.estimacionSiguienteEventoDifuso(interseccion.punto, r.d, 
                        interseccion.geometria, interseccion.normal);
                    rgb = rgb + c;
                }
            }
            imagen.cambiarPixel(i,j,rgb/rayos);
        }
    }
    return imagen;
}

// Genera una imagen con la luz directa y la luz indirecta con superficies difusas de la escena
Imagen Camara::generarImagenDifuso(Escena e, int ancho, int alto, int resolucion, int rayos, int N) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    RGB c = e.ecuacionRenderDifuso(interseccion.punto, r.d, 
                        interseccion.geometria, interseccion.normal, N);
                    rgb = rgb + c;
                }
            }
            imagen.cambiarPixel(i,j,rgb/rayos);
        }
    }
    return imagen;
}

// Genera una imagen completa (path Tracer) a partir de la escena
Imagen Camara::generarImagen(Escena e, int ancho, int alto, int resolucion, int rayos) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    RGB c = e.ecuacionRender(interseccion.punto, r.d, interseccion.geometria, interseccion.normal);
                    rgb = rgb + c;
                }
            }
            imagen.cambiarPixel(i,j,rgb/rayos);
        }
    }
    return imagen;
}

// Genera una imagen completa (path Tracer) a partir de la escena con paralelizacion
Imagen Camara::generarImagenParalela(Escena e, int ancho, int alto, int resolucion, int rayos, int numThreads){
    vector<thread> threads;
    std::mutex mutexImagenes;
    Imagen* imagen = new Imagen(ancho, alto, resolucion);
    for (int i = 0; i < numThreads; i++) {
        // cada hilo devuelve un subconjunto de la imagen
        threads.push_back(std::thread([this, imagen, e, ancho, alto, resolucion, rayos, numThreads, i]() {
        // Llamar a la función con los argumentos
        this->generarImagenParalelaAux(e, ancho, alto, resolucion, rayos, imagen, numThreads, i);
        }));
    }
    for (std::thread& thread : threads) {thread.join();}
    return *imagen;
}

// Genera una imagen completa (photon Mapper) a partir de la escena
Imagen Camara::generarImagenFotones(Escena e, int ancho, int alto, int resolucion, int rayos, 
                MapaFotones mapa, int kFotones, double radio, bool guardar, Kernel* kernel) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapa, kFotones, radio, guardar, kernel);
                }
            }
            imagen.cambiarPixel(i,j,rgb/rayos);
        }
    }
    return imagen;
}

// Genera una imagen completa (photon Mapper) con causticas separadas
Imagen Camara::generarImagenMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
                MapaFotones mapa, int kFotones, double radio,
                MapaFotones mapaCausticas, int kFotonesCausticas, double radioCausticas, 
                bool guardar, Kernel* kernel) {
    Imagen imagen = Imagen(ancho, alto, resolucion);
    for (int i = 1; i <= ancho; i++) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapa, kFotones, radio, guardar, kernel);
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapaCausticas, kFotonesCausticas, radioCausticas, true, kernel);
                }
            }
            imagen.cambiarPixel(i,j,rgb/rayos);
        }
    }
    return imagen;
}

// Genera una imagen completa (photon Mapper) a partir de la escena con paralelizacion
Imagen Camara::generarImagenFotonesParalela(Escena e, int ancho, int alto, int resolucion, int rayos, 
                MapaFotones mapa, int kFotones, double radio, bool guardar, int numThreads, Kernel* kernel, double sigma) {
    vector<thread> threads;
    int anchoHilo = ancho/numThreads;
    Imagen* imagen = new Imagen(ancho, alto, resolucion);
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread([this, e, ancho, alto, resolucion, rayos, mapa, kFotones, radio, guardar, 
            numThreads, i, imagen, kernel, sigma]() {
        this->generarImagenFotonesParalelaAux(e, ancho, alto, resolucion, rayos, imagen, mapa, kFotones, radio, 
            guardar, numThreads, i, kernel, sigma);
        }));
    }
    for (std::thread& thread : threads) {thread.join();}
    return *imagen;
}

// Genera una imagen completa (photon Mapper) con causticas separadas
Imagen Camara::generarImagenFotonesParalelaMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
                MapaFotones mapa, int kFotones, double radio,
                MapaFotones mapaCausticas, int kFotonesCausticas, double radioCausticas, 
                bool guardar, int numThreads, Kernel* kernel) {
    vector<thread> threads;
    int anchoHilo = ancho/numThreads;
    Imagen* imagen = new Imagen(ancho, alto, resolucion);
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread([this, e, ancho, alto, resolucion, rayos, mapa, kFotones, radio, 
            mapaCausticas, kFotonesCausticas, radioCausticas, guardar, numThreads, i, imagen, kernel]() {
        this->generarImagenFotonesParalelaAuxMapaCaustico(e, ancho, alto, resolucion, rayos, imagen, mapa, kFotones, radio, 
            mapaCausticas, kFotonesCausticas, radioCausticas, guardar, numThreads, i, kernel);
        }));
    }
    for (std::thread& thread : threads) {thread.join();}
    return *imagen;
}

// Genera una imagen completa a partir de dos mapas de fotones (progresivo)
Imagen Camara::generarImagenFotonesParalelaProgresivo(Escena e, int ancho, int alto, int resolucion, int rayos, 
                MapaFotones mapaCausticas, MapaFotones mapa_base, int emitidos_base, int iteraciones, 
                int emitidos_progresivo, double alpha, int kFotones, 
                double radio, bool guardar, int numThreads) {
    vector<thread> threads;
    int anchoHilo = ancho/numThreads;
    Imagen* imagen = new Imagen(ancho, alto, resolucion);
    vector<MapaFotones> mapas;
    for (int i = 0; i < iteraciones; i++) {
        mapas.push_back(e.generarMapaFotones(emitidos_progresivo, guardar));
    }

    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread([this, e, ancho, alto, resolucion, rayos, mapa_base, emitidos_base, 
            iteraciones, emitidos_progresivo, alpha, kFotones, radio, guardar, numThreads, i, imagen, mapaCausticas, mapas]() {
        this->generarImagenFotonesParalelaAuxProgresivo(e, ancho, alto, resolucion, rayos, imagen, mapaCausticas, mapa_base, emitidos_base, 
            iteraciones, emitidos_progresivo, mapas, alpha, kFotones, radio, guardar, numThreads, i);
        }));
    }
    for (std::thread& thread : threads) {thread.join();}
    return *imagen;
}

// Funcion auxiliar
void mostrarInformacionETA(double porcentaje, double tiempo) {
    double tiempoRestante = tiempo * (100.0 - porcentaje) / porcentaje;
    int horas = tiempoRestante / 3600;
    int minutos = (tiempoRestante - horas * 3600) / 60;
    int segundos = tiempoRestante - horas * 3600 - minutos * 60;
    cout << "Progreso aproximado: " << porcentaje << "%";
    if (horas > 0) {
        cout << " - ETA: " << horas << "h " << minutos << "m " << segundos << "s" << endl;
    } else if (minutos > 0) {
        cout << " - ETA: " << minutos << "m " << segundos << "s" << endl;
    } else if (segundos > 0) {
        cout << " - ETA: " << segundos << "s" << endl;
    } else {
        cout << " - ETA: menos de un segundo" << endl;
    }
}

// Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
void Camara::generarImagenParalelaAux(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, int numThreads, int idHilo){
    // Dividir la imagen a lo ancho
    int altoHilo = alto;
    int i = idHilo +1;
    auto start = std::chrono::high_resolution_clock::now();
    while (i <= ancho) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    RGB c = e.ecuacionRender(interseccion.punto, r.d, interseccion.geometria, interseccion.normal);
                    rgb = rgb + c;
                }
            }
            imagen->cambiarPixel(i,j,rgb/rayos);
        }
        if (idHilo == 0) {
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            double porcentaje = (double)i/ancho*100;
            mostrarInformacionETA(porcentaje, elapsed.count());
        }
        i += numThreads;
    }
}

// Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
void Camara::generarImagenFotonesParalelaAux(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapa, int kFotones, double radio, 
            bool guardar, int numThreads, int idHilo, Kernel* kernel, double sigma) {
    // semilla aleatoria para cada hilo
    srand(idHilo);
    // Dividir la imagen a lo ancho
    int i = idHilo + 1;
    auto start = std::chrono::high_resolution_clock::now();
    while (i <= ancho) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapa, kFotones, radio, guardar, kernel, sigma);
                }
            }
            imagen->cambiarPixel(i,j,rgb/rayos);
        }
        if (idHilo == 0) {
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            double porcentaje = (double)i/ancho*100;
            mostrarInformacionETA(porcentaje, elapsed.count());
        }
        i += numThreads;
    }
}

// Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
void Camara::generarImagenFotonesParalelaAuxMapaCaustico(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapa, int kFotones, double radio,
            MapaFotones mapaCausticas, int kFotonesCausticas, double radioCausticas, 
            bool guardar, int numThreads, int idHilo, Kernel* kernel) {
    srand(idHilo*resolucion+radio);
    // Dividir la imagen a lo ancho
    int i = idHilo +1;
    auto start = std::chrono::high_resolution_clock::now();
    while (i <= ancho) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapa, kFotones, radio, guardar, kernel);
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapaCausticas, kFotonesCausticas, radioCausticas, true, kernel);
                }
            }
            imagen->cambiarPixel(i,j,rgb/rayos);
        }
        if (idHilo == 0) {
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            double porcentaje = (double)i/ancho*100;
            mostrarInformacionETA(porcentaje, elapsed.count());
        }
        i += numThreads;
    }
}

// Funcion auxiliar para generar una imagen completa a partir de la escena con paralelizacion
void Camara::generarImagenFotonesParalelaAuxProgresivo(Escena e, int ancho, int alto, int resolucion, int rayos, 
            Imagen* imagen, MapaFotones mapaCausticas, MapaFotones mapa_base, int emitidos_base, int iteraciones, 
            int emitidos_progresivo, vector<MapaFotones> mapas, double alpha, int kFotones, 
            double radio, bool guardar, int numThreads, int idHilo) {
    srand(idHilo*resolucion+radio);
    // Dividir la imagen a lo ancho
    int i = idHilo +1;
    auto start = std::chrono::high_resolution_clock::now();
    while (i <= ancho) {
        for (int j = 1; j <= alto; j++) {
            RGB rgb = RGB({0.0,0.0,0.0});
            for (int k = 0; k < rayos; k++) {
                Rayo r = generarRayo(i,j,ancho,alto);
                Interseccion interseccion = e.interseccion(r);
                if (interseccion.numIntersecciones > 0 && interseccion.distancia > 0.0) {
                    rgb = rgb + e.ecuacionRenderFotonesProgresivo(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapa_base, emitidos_base, iteraciones, emitidos_progresivo, mapas, 
                        alpha, kFotones, radio, guardar);
                    rgb = rgb + e.ecuacionRenderFotones(interseccion.punto, r.d, interseccion.geometria, 
                        interseccion.normal, mapaCausticas, kFotones, radio, true);
                }
            }
            if (max(rgb/rayos) > 1.0) {
                rgb = rgb/max(rgb/rayos);
            }
            imagen->cambiarPixel(i,j,rgb/rayos);
        }
        if (idHilo == 0) {
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            double porcentaje = (double)i/ancho*100;
            mostrarInformacionETA(porcentaje, elapsed.count());
        }
        i += numThreads;
    }
}
