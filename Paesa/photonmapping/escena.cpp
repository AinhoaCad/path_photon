// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "escena.h"
#include "aleatorio.h"

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

const double ERROR = 1.0e-6;

// ------------------------------ Constructores ------------------------------
// Constructor por defecto
Escena::Escena() {
    this->pmax = Punto(-INFINITY,-INFINITY,-INFINITY);
    this->pmin = Punto(INFINITY,INFINITY,INFINITY);
    this->objetos = vector<Geometria*>();
    this->luces = vector<LuzPuntual*>();
}

// Constructor con objetos
Escena::Escena(vector<Geometria*> objetos) {
    this->pmax = Punto(-INFINITY,-INFINITY,-INFINITY);
    this->pmin = Punto(INFINITY,INFINITY,INFINITY);
    this->objetos = objetos;
    this->luces = vector<LuzPuntual*>();
    for (int i = 0; i < objetos.size(); i++) {
        Punto p0 = objetos[i]->cajaEnvolventeMin();
        Punto p1 = objetos[i]->cajaEnvolventeMax();
        if (p0.h[0] < pmin.h[0]) pmin.h[0] = p0.h[0];
        if (p0.h[1] < pmin.h[1]) pmin.h[1] = p0.h[1];
        if (p0.h[2] < pmin.h[2]) pmin.h[2] = p0.h[2];
        if (p1.h[0] > pmax.h[0]) pmax.h[0] = p1.h[0];
        if (p1.h[1] > pmax.h[1]) pmax.h[1] = p1.h[1];
        if (p1.h[2] > pmax.h[2]) pmax.h[2] = p1.h[2];
    }
}

// ------------------------------ Funciones ------------------------------
// Funcion para anyadir objetos a la escena
void Escena::anyadir(Geometria* objeto) {
    objetos.push_back(objeto);
    Punto p0 = objeto->cajaEnvolventeMin();
    Punto p1 = objeto->cajaEnvolventeMax();
    if (p0.h[0] < pmin.h[0]) pmin.h[0] = p0.h[0];
    if (p0.h[1] < pmin.h[1]) pmin.h[1] = p0.h[1];
    if (p0.h[2] < pmin.h[2]) pmin.h[2] = p0.h[2];
    if (p1.h[0] > pmax.h[0]) pmax.h[0] = p1.h[0];
    if (p1.h[1] > pmax.h[1]) pmax.h[1] = p1.h[1];
    if (p1.h[2] > pmax.h[2]) pmax.h[2] = p1.h[2];
}

// Funcion para anyadir luces a la escena
void Escena::anyadirLuz(LuzPuntual* luz) {
    luces.push_back(luz);
}

// Devuelve la interseccion mas cercana de un rayo con los objetos de la escena
Interseccion Escena::interseccion(Rayo r) {
    Interseccion interseccion = Interseccion();
    if (this->cajaEnvolvente().interseccion(r).numIntersecciones == 0) return interseccion;
    for (int i = 0; i < objetos.size(); i++) {
        Interseccion interseccionObjeto = objetos[i]->interseccion(r);
        if (interseccionObjeto.numIntersecciones > 0 && interseccionObjeto.distancia > ERROR ) {
            if (interseccion.distancia == 0.0 ||interseccionObjeto.distancia < interseccion.distancia) 
                interseccion = interseccionObjeto;
        }
    }
    return interseccion;
}

// Se mantienen las funciones realizadas de manera incremental para ilustrar
// el proceso de desarrollo del proyecto

// Devuelve el color difuso en un punto de la escena con geometrias difusas
RGB Escena::estimacionSiguienteEventoDifuso(Punto x, Direccion wo, Geometria* g, Direccion n) {
    RGB L = RGB({0.0,0.0,0.0});
    for (int i = 0; i < luces.size(); i++) {
        Direccion wi = (luces[i]->p - x).normalizado();
        double norma = (luces[i]->p - x)*(luces[i]->p - x);
        double coseno = n*wi;
        RGB fr = g->fr_solo_difuso(x, wo, wi, n);
        if (coseno > 0) {
            Interseccion interseccion = this->interseccion(Rayo(luces[i]->p,-wi));
            if (interseccion.numIntersecciones > 0 && interseccion.distancia >= sqrt(norma) - ERROR) {
                L = L + (coseno*fr)*(luces[i]->c/norma);
            }
        }
    }
    return L;
}

// Devuelve el color en un punto de la escena con geometrias difusas
RGB Escena::ecuacionRenderDifuso(Punto x, Direccion wo, Geometria* g, Direccion n, int N) {
    if (g->esLuzArea()) return g->k_e;
    if (N == 0) return RGB({0.0,0.0,0.0});
    Direccion normal = n;
    if (wo*n > 0.0) normal = -normal;
    DireccionMuestraAleatoria d = DireccionMuestraAleatoria(normal,x);
    Direccion wi = d.cosenoUniforme();
    Interseccion interseccion = this->interseccion(Rayo(x,wi));
    RGB luzDirecta = estimacionSiguienteEventoDifuso(x, wo, g, normal);
    if (interseccion.numIntersecciones == 0 ||  interseccion.distancia <= 0.0) 
        return luzDirecta;
    RGB fr = g->fr_solo_difuso(x, wo, wi, normal);
    return luzDirecta
        + M_PI*fr*ecuacionRenderDifuso(interseccion.punto, wi, interseccion.geometria, interseccion.normal, N-1);
}

// Devuelve la luz directa en un punto de la escena sobre una geometria difusa
RGB Escena::estimacionSiguienteEvento(Punto x, Direccion wo, Geometria* g, Direccion n, double sigma) {
    RGB L = RGB({0.0,0.0,0.0});
    for (int i = 0; i < luces.size(); i++) {
        Direccion wi = (luces[i]->p - x).normalizado();
        double norma = (luces[i]->p - x)*(luces[i]->p - x);
        double coseno = n*wi;
        RGB fr = g->fr_solo_difuso(x, wo, wi, n);
        if (coseno > 0) {
            Interseccion interseccion = this->interseccion(Rayo(luces[i]->p,-wi));
            if (interseccion.numIntersecciones > 0 && interseccion.distancia >= sqrt(norma) - 0.0001) {
                if (sigma == 0.0) L = L + (coseno*fr)*(luces[i]->c/norma);
                else L = L + (coseno*fr)*(luces[i]->c/norma)*pow(M_E, -sigma*norma);
            }
        }
    }
    return L;
}

// Devuelve el color en un punto de la escena con geometrias especulares
RGB Escena::ecuacionRender(Punto x, Direccion wo, Geometria* g, Direccion n) {
    if (g->esLuzArea()) return g->k_e;
    EVENTO evento = g->evento_aleatorio();
    Direccion wi;
    Direccion normal = n;
    if (wo*n > 0.0) normal = -normal;
    DireccionMuestraAleatoria d = DireccionMuestraAleatoria(normal,x);
    Interseccion interseccion;
    double norma;
    RGB fr, L;
    switch (evento) {
        case EVENTO_DIFUSO:
            wi = d.cosenoUniforme();
            interseccion = this->interseccion(Rayo(x,wi));
            if (interseccion.numIntersecciones == 0 || interseccion.distancia <= 0.0) return estimacionSiguienteEvento(x, wo, g, normal);
            fr = g->fr_difuso(x, wi, wo, n);
            L = fr*ecuacionRender(interseccion.punto, wi, interseccion.geometria, interseccion.normal)
                    + estimacionSiguienteEvento(x, wo, g, normal);
            break; 
        case EVENTO_ESPECULAR:
            wi = g->reflejar(wo, normal);
            interseccion = this->interseccion(Rayo(x,wi));
            if (interseccion.numIntersecciones == 0 || interseccion.distancia <= -0.01) return RGB({0.0,0.0,0.0});
            fr = g->fr_especular(x, wi, wo, normal);
            L = fr*ecuacionRender(interseccion.punto, wi, interseccion.geometria, interseccion.normal);
            break;
        case EVENTO_REFRACCION:
            wi = g->refractar(wo, n);
            interseccion = this->interseccion(Rayo(x,wi));
            if (interseccion.numIntersecciones == 0 || interseccion.distancia <= -0.01) return RGB({0.0,0.0,0.0});
            fr = g->fr_refraccion(x, wi, wo, normal);
            L = fr*ecuacionRender(interseccion.punto, wi, interseccion.geometria, interseccion.normal);
            break;
        case EVENTO_EMISION:
            L = g->k_e;
            break;
        default: // EVENTO_ABSORCION
            L = RGB_NEGRO;
            break;
    }
    return L;
}

MapaFotones Escena::generarMapaFotones(int numPaseos, bool guardar, double sigma) {
    list<Foton> fotones;
    double totalEmision = 0.0;
    for (LuzPuntual* luz : luces) totalEmision += max(luz->c);
    for (LuzPuntual* luz : luces) {
        int numFotones = (int)(numPaseos*max(luz->c)/totalEmision);
        for (int j = 0; j < numFotones; j++) {
            Direccion d = muestraAnguloSolido();
            Rayo r = Rayo(luz->p, d);
            primerReboteFoton(r, 4*M_PI*luz->c/numFotones, fotones, fotones, guardar, sigma);
        }
    }
    MapaFotones mapa = constructorMapaFotones(fotones);
    return mapa;  
}

void Escena::generarMapasFotonesCausticas(int numPaseos, MapaFotones& mapa, 
            MapaFotones& mapaCausticas, bool guardar) {
    list<Foton> fotones;
    list<Foton> causticos;
    double totalEmision = 0.0;
    for (LuzPuntual* luz : luces) totalEmision += max(luz->c);
    for (LuzPuntual* luz : luces) {
        int numFotones = (int)(numPaseos*max(luz->c)/totalEmision);
        for (int j = 0; j < numFotones; j++) {
            Direccion d = muestraAnguloSolido();
            Rayo r = Rayo(luz->p, d);
            primerReboteFoton(r, 4*M_PI*luz->c/numFotones, fotones, causticos, guardar);
        }
    }
    mapa = constructorMapaFotones(fotones);
    mapaCausticas = constructorMapaFotones(causticos);
}

void Escena::primerReboteFoton(Rayo r, RGB luz, list<Foton>& fotones, 
            list<Foton>& causticos, bool guardar, double sigma) {
    Interseccion i = interseccion(r);
    RGB radiancia = luz;
    Direccion wo = r.d, wi;
    EVENTO ev;
    bool caustico = false;
    if (i.numIntersecciones > 0) { // Si interseca la escena
        if (i.geometria->esLuzArea()) {
            if (guardar) {
                Foton f = Foton(i.punto, wo, radiancia);
                fotones.push_back(f);
            }
            return;
        }
        ev = i.geometria->evento_aleatorio();
        double norma = (r.p - i.punto)*(r.p - i.punto);
        RGB brdf = luz/norma;
        if (ev == EVENTO_ABSORCION) return;
        else if (ev == EVENTO_DIFUSO) {
            Direccion normal = i.normal;
            if (wo*normal > 0.0) normal = -normal;
            DireccionMuestraAleatoria d = DireccionMuestraAleatoria(normal, i.punto);
            wi = d.cosenoUniforme();
            brdf = brdf *abs(wi*normal)
                    * i.geometria->fr_difuso(i.punto, wi, wo, normal);
            if (guardar) { // Guardamos la luz directa
                Foton f = Foton(i.punto, wo, brdf);
                fotones.push_back(f);
            }
        } else if (ev == EVENTO_ESPECULAR) {
            caustico = true;
            Direccion normal = i.normal;
            if (wo*normal > 0.0) normal = -normal;
            wi = i.geometria->reflejar(wo, normal);
            brdf = brdf *abs(wi*normal)
                    * i.geometria->fr_especular(i.punto, wi, wo, normal);
        } else if (ev == EVENTO_REFRACCION) {
            caustico = true;
            Direccion normal = i.normal;
            wi = i.geometria->refractar(wo, normal);
            brdf = brdf *abs(wi*normal)
                    * i.geometria->fr_refraccion(i.punto, wi, wo, normal);
        }
        // Sigue el camino del foton
        brdf = brdf * pow(M_E, -sigma*norma);
        calcularCaminoFoton(Rayo(i.punto, wi), brdf, fotones, causticos, caustico, sigma);
    }
}

void Escena::calcularCaminoFoton(Rayo r, RGB luz, list<Foton>& fotones,
            list<Foton>& causticos, bool caustico, double sigma) {
    Interseccion i = interseccion(r);
    RGB radiancia = luz;
    Direccion wo = r.d, wi;
    EVENTO ev;
    bool esCaustico = caustico;
    double norma = (r.p - i.punto)*(r.p - i.punto);
    while (i.numIntersecciones > 0) { // Si interseca la escena
        radiancia = radiancia * pow(M_E, -sigma*norma);
        if (i.geometria->esLuzArea()) {
            Foton f = Foton(i.punto, wo, radiancia);
            fotones.push_back(f);
            return;
        }
        ev = i.geometria->evento_aleatorio();
        if (ev == EVENTO_ABSORCION) return;
        else if (ev == EVENTO_DIFUSO) {
            Direccion normal = i.normal;
            if (wo*normal > 0.0) normal = -normal;
            DireccionMuestraAleatoria d = DireccionMuestraAleatoria(normal, i.punto);
            wi = d.cosenoUniforme();
            Foton f = Foton(i.punto, wo, radiancia);
            if (esCaustico) causticos.push_back(f);
            else fotones.push_back(f);
            esCaustico = false;
            radiancia = radiancia*abs(wi*normal)
                    * i.geometria->fr_difuso(i.punto, wi, wo, normal);
        } else if (ev == EVENTO_ESPECULAR) {
            esCaustico = true;
            Direccion normal = i.normal;
            if (wo*normal > 0.0) normal = -normal;
            wi = i.geometria->reflejar(wo, normal);
            radiancia = radiancia*abs(wi*normal)
                    * i.geometria->fr_especular(i.punto, wi, wo, i.normal);
        } else if (ev == EVENTO_REFRACCION) {
            esCaustico = true;
            wi = i.geometria->refractar(wo, i.normal);
            radiancia = radiancia*abs(wi*i.normal)
                    * i.geometria->fr_refraccion(i.punto, wi, wo, i.normal);
        }
        // Sigue el camino del foton
        i = interseccion(Rayo(i.punto, wi));
        norma = (r.p - i.punto)*(r.p - i.punto);
    }
}

RGB Escena::ecuacionRenderFotones(Punto x, Direccion wo, Geometria* geo, Direccion n, 
            MapaFotones mapa, int kFotones, double radio, bool guardar, Kernel* kernel, double sigma) {
    if (geo->esLuzArea()) return geo->k_e;
    Direccion normal = n;
    double radioFotonMasLejano = 0.0;
    double radioFoton = 0.0;
    Punto posFoton;
    RGB L= RGB({0.0,0.0,0.0});
    EVENTO evento = geo->evento_aleatorio();
    Punto p = x;
    Geometria* g = geo;
    // Seguimos hasta llegar a una superficie difusa, simulando el camino del foton
    while (evento == EVENTO_ESPECULAR || evento == EVENTO_REFRACCION) {
        if (evento == EVENTO_ESPECULAR) {
            if (wo*normal > 0.0) normal = -normal;
            wo = g->reflejar(wo, normal);
        } else if (evento == EVENTO_REFRACCION) {
            wo = g->refractar(wo, normal);
        }
        Interseccion i = this->interseccion(Rayo(p,wo));
        if (i.numIntersecciones == 0 ) {
            evento = EVENTO_ABSORCION;
        } else {
            p = i.punto;
            g = i.geometria;
            normal = i.normal;
            evento = g->evento_aleatorio();
        }
    }
    if (evento == EVENTO_DIFUSO) {
        if (wo*normal > 0.0) normal = -normal;
        vector<const Foton*> fotones = mapa.nearest_neighbors(p, kFotones, radio);
        for (const Foton* f : fotones) {
            posFoton = f->posicion;
            radioFoton = (posFoton - p).modulo();
            if (radioFoton > radioFotonMasLejano) radioFotonMasLejano = radioFoton;
        }
        for (const Foton* f : fotones) {
            Direccion wi = f->direccion;
            double coseno = -normal*wi;
            if (coseno > 0.0) {
                posFoton = f->posicion;
                L = L + g->fr_difuso(p, wi, wo, normal)*f->flujo
                    *kernel->evaluar((posFoton - p).modulo(), radioFotonMasLejano);
            }
        }
        // Estimacion de la luz directa
        if (!guardar) L = L + estimacionSiguienteEvento(p, wo, g, normal, sigma);
    } else if (evento == EVENTO_EMISION) {
        L = L + g->k_e;
    }
    return L;
}

RGB Escena::ecuacionRenderFotonesProgresivo(Punto x, Direccion wo, Geometria* geo, Direccion n, 
            MapaFotones mapa_base, int emitidos_base, int iteraciones, int emitidos_progresivo,
            vector<MapaFotones> mapas, double alpha, int kFotones, double radio, bool guardar) {
    if (geo->esLuzArea()) return geo->k_e;
    Direccion normal = n;
    double radioFotonMasLejano = 0.0;
    double radioFoton = 0.0;
    Punto posFoton;
    RGB L= RGB({0.0,0.0,0.0});
    EVENTO evento = geo->evento_aleatorio();
    Punto p = x;
    Geometria* g = geo;
    // Seguimos hasta llegar a una superficie difusa, simulando el camino del foton
    while (evento == EVENTO_ESPECULAR || evento == EVENTO_REFRACCION) {
        if (evento == EVENTO_ESPECULAR) {
            if (wo*normal > 0.0) normal = -normal;
            wo = g->reflejar(wo, normal);
        } else if (evento == EVENTO_REFRACCION) {
            wo = g->refractar(wo, normal);
        }
        Interseccion i = this->interseccion(Rayo(p,wo));
        if (i.numIntersecciones == 0 ) {
            evento = EVENTO_ABSORCION;
        } else {
            p = i.punto;
            g = i.geometria;
            normal = i.normal;
            evento = g->evento_aleatorio();
        }
    }
    if (evento == EVENTO_DIFUSO) {
        if (wo*normal > 0.0) normal = -normal;
        int emitidos_total = emitidos_base;
        double radio_actual = radio;
        vector<const Foton*> fotones_base = mapa_base.nearest_neighbors(p, kFotones, radio_actual);

        for (const Foton* f : fotones_base) {
            posFoton = f->posicion;
            radioFoton = (posFoton - p).modulo();
            if (radioFoton > radioFotonMasLejano) radioFotonMasLejano = radioFoton;
        }
        int n_base = fotones_base.size();
        if (n_base == 0) n_base = 1;

        for (const Foton* f : fotones_base) { // Calculamos la radiancia de los fotones base
            Direccion wi = f->direccion;
            double coseno = -normal*wi;
            if (coseno > 0.0) {
                posFoton = f->posicion;
                L = L + g->fr_difuso(p, wi, wo, normal)*f->flujo*emitidos_base;
            }
        }

        for (int i = 0; i < iteraciones; i++) {
            vector<const Foton*> fotones_progresivo = mapas[i].nearest_neighbors(p, kFotones, radio_actual);
            int n_progresivo = fotones_progresivo.size();
            if (n_progresivo == 0) n_progresivo = 1;
            double constante_reduccion = (double)(n_base + alpha * n_progresivo) / (n_base + n_progresivo);
            radio_actual = radio_actual * sqrt(constante_reduccion);
            n_base = n_base + alpha * n_progresivo;

            for (const Foton* f : fotones_progresivo) { // Calculamos la radiancia de los fotones progresivo
                Direccion wi = f->direccion;
                double coseno = -normal*wi;
                if (coseno > 0.0) {
                    posFoton = f->posicion;
                    L = L + g->fr_difuso(p, wi, wo, normal)*f->flujo*emitidos_progresivo;
                }
            }
            L = L * constante_reduccion;
            emitidos_total += emitidos_progresivo;
        }
        // Ponderamos la radiancia
        L = L / (M_PI*radio_actual*radio_actual*(emitidos_total));
        // Estimacion de la luz directa
        if (!guardar) L = L + estimacionSiguienteEvento(p, wo, g, normal);
    } else if (evento == EVENTO_EMISION) {
        L = L + g->k_e;
    }
    return L;
}

// Devuelve la caja que contiene a todos los objetos de la escena
Caja Escena::cajaEnvolvente() {
    return Caja(pmin, pmax);
}

// Establece la caja que contiene a todos los objetos de la escena
void Escena::establecerCajaEnvolvente(Punto p0, Punto p1) {
    this->pmin = p0;
    this->pmax = p1;
}

// -------------------------- Escenas de prueba --------------------------
// Luz puntual -----------------------------------------------------------
Escena CornellBox() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, RGB_ROSA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_CIAN, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0), RGB_BLANCO);
    escena.anyadirLuz(luz);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBox2() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    RGB cian = RGB_CIAN;
    RGB gris = RGB_GRIS;
    double m = max(cian) + max(gris);
    cian = cian / m;
    gris = gris / m;
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, cian, gris, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_NEGRO, RGB_BLANCO*0.1, RGB_BLANCO*0.9, RGB_NEGRO, 1.5, N_AIRE); 

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0), RGB_BLANCO);
    escena.anyadirLuz(luz);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxDosLuces() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* sphere       = new Esfera(Punto(0.0,0.0,0.0), 0.3, RGB_MAGENTA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(sphere);

    LuzPuntual* luz1 = new LuzPuntual(Punto(-0.4,0.5,0.0), RGB_BLANCO*0.9);
    escena.anyadirLuz(luz1);
    LuzPuntual* luz2 = new LuzPuntual(Punto(0.4,0.5,0.0), RGB_BLANCO*0.9);
    escena.anyadirLuz(luz2);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxColorBleeding() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* sphere       = new Esfera(Punto(0.0,-0.7,0.0), 0.3, RGB_AZUL, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); 
    Esfera* sphere2       = new Esfera(Punto(-0.65,-0.7,0.0), 0.3, RGB_MAGENTA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Esfera* sphere3       = new Esfera(Punto(0.65,-0.7,0.0), 0.3, RGB_AMARILLO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(sphere);
    escena.anyadir(sphere2);
    escena.anyadir(sphere3);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0), RGB_BLANCO);
    escena.anyadirLuz(luz);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxTexturas() {
    Imagen* textura      = new Imagen("texturas/tablero.ppm");
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), textura); 
    //Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, textura);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_CIAN, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0), RGB_BLANCO);
    escena.anyadirLuz(luz);
    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxCausticasPuntual() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* sphere       = new Esfera(Punto(0.0,-0.6,0.0), 0.3, RGB_NEGRO, RGB_GRIS*0.1, RGB_BLANCO*0.9, RGB_NEGRO, 1.5, N_AIRE);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(sphere);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0.1), RGB_BLANCO);
    escena.anyadirLuz(luz);
    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

// Luz area --------------------------------------------------------------
Escena CornellBoxLuzArea() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO, RGB_BLANCO); 
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, RGB_ROSA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_CIAN, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxLuzArea2() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO, RGB_BLANCO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    RGB cian = RGB_CIAN;
    RGB gris = RGB_GRIS;
    double m = max(cian) + max(gris);
    cian = cian / m;
    gris = gris / m;
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, cian, gris, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_NEGRO, RGB_GRIS*0.1, RGB_BLANCO*0.9, RGB_NEGRO, 1.5, N_AIRE); 

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxSombraSuave() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO, RGB_BLANCO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* sphere       = new Esfera(Punto(0.0,-0.4,0.0), 0.3, RGB_MAGENTA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(sphere);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxCausticas() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO, RGB_BLANCO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* sphere       = new Esfera(Punto(0.0,-0.6,0.0), 0.3, RGB_NEGRO, RGB_GRIS*0.1, RGB_BLANCO*0.9, RGB_NEGRO, 1.5, N_AIRE);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(sphere);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

Escena CornellBoxLuzPeque() {
    Plano* leftPlane     = new Plano(Punto(-1,0,0), Direccion(1,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(1,0,0), Direccion(-1,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,1,0), Direccion(0,-1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); 
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Esfera* leftSphere   = new Esfera(Punto(-0.5,-0.7,0.25), 0.3, RGB_ROSA, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_CIAN, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Triangulo* luzArea   = new Triangulo(Punto(0.9,0.99,0),Punto(0,0.99,0.9),Punto(-0.3,0.99,-0.3), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO, RGB_BLANCO);

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftSphere);
    escena.anyadir(rightSphere);
    escena.anyadir(luzArea);

    escena.establecerCajaEnvolvente(Punto(-1,-1,-1), Punto(1,1,1));
    return escena;
}

// Escenas elaboradas ----------------------------------------------------
Escena OtrasGeometrias() {
    Cilindro* cilindro = new Cilindro(Punto(-0.7,-0.7,0.25), Direccion(0,1,0), 0.25, RGB({0.0,0.0,1.0}), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Cono* cono = new Cono(Punto(0.4,-0.5,0), Direccion(1,0,0), M_PI/32, RGB({0.0,1.0,1.0}), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane = new Plano(Punto(0,0,1), Direccion(0,0,-1), RGB({0.5,0.5,0.8}), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  
    Plano* bottomPlane   = new Plano(Punto(0,-1,0), Direccion(0,1,0), RGB_GRIS, RGB_GRIS, RGB_NEGRO, RGB_NEGRO);
    CilindroFinito* cilindroFinito = new CilindroFinito(Punto(0,-1,0.5), Direccion(0,1,0), 0.15, 0.25, RGB_ROJO*0.5, RGB_NEGRO, RGB_NEGRO, RGB_ROJO*0.5);
    Disco* disco = new Disco(Punto(0,-0.75,0.5), Direccion(0,1,0), 0.15, RGB_ROJO*0.5, RGB_NEGRO, RGB_NEGRO, RGB_ROJO*0.5);
    Caja* caja = new Caja(Punto(-0.5,-1,-0.3), Punto(-0.25,-0.6,0.1), RGB_NEGRO, RGB_GRIS, RGB_GRIS, RGB_NEGRO, 1.5);
    Triangulo* triangulo = new Triangulo(Punto(0.5,-1,-0.3), Punto(0.75,-0.6,0.1), Punto(0.25,-0.6,0.1), RGB({0.5,0.5,1.0}), RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    ConoFinito* conoFinito = new ConoFinito(Punto(0.8,-0.5,-0.5), Direccion(0,1,0), M_PI/32, 0.4, RGB({0.5,1.0,0.5})*0.5, RGB_GRIS, RGB_NEGRO, RGB_NEGRO);
    Esfera* sphere       = new Esfera(Punto(0.5,0.3,0.0), 0.25, RGB_NEGRO, RGB_BLANCO, RGB_NEGRO, RGB_NEGRO, 1.5, N_AIRE);
    Escena escena = Escena();
    escena.anyadir(cilindro);
    escena.anyadir(cono);
    escena.anyadir(cilindroFinito);
    escena.anyadir(backPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(disco);
    escena.anyadir(caja);
    escena.anyadir(triangulo);
    escena.anyadir(conoFinito);
    escena.anyadir(sphere);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0.5,0), RGB_BLANCO);
    escena.anyadirLuz(luz);

    escena.establecerCajaEnvolvente(Punto(-10,-10,-10), Punto(10,10,10));
    return escena;
}

Escena Bonita(){
    RGB grisclaro = RGB_GRIS/4;
    Plano* leftPlane     = new Plano(Punto(-2,0,0), Direccion(2,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(2,0,0), Direccion(-2,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,2,0), Direccion(0,-2,0), grisclaro, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-2,0), Direccion(0,2,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,2), Direccion(0,0,-2), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  

    ConoFinito* leftCone = new ConoFinito(Punto(-1, 0.5, 1), Direccion(0,-1,0), M_PI/32, 2.5, RGB({0.78,0.78,0.78}), RGB_BLANCO, RGB_NEGRO, RGB_NEGRO);
    Esfera* rightSphere  = new Esfera(Punto(0.5,-0.7,-0.25), 0.3, RGB_VERDE*0.3, RGB_GRIS*0.1, RGB_BLANCO*0.6, RGB_NEGRO, 1.5, N_AIRE); 

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftCone);
    escena.anyadir(rightSphere);

    // Color calido anaranjado
    RGB calido = RGB({1, 0.4, 0});

    LuzPuntual* luz = new LuzPuntual(Punto(0,-1.75,0), calido);
    escena.anyadirLuz(luz);

    escena.establecerCajaEnvolvente(Punto(-2,-2,-2), Punto(2,2,2));
    return escena;
}

Escena BonitaLuzArea(){
    RGB grisclaro = RGB_GRIS/4;
    Plano* leftPlane     = new Plano(Punto(-2,0,0), Direccion(2,0,0), RGB_ROJO, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* rightPlane    = new Plano(Punto(2,0,0), Direccion(-2,0,0), RGB_VERDE, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* topPlane      = new Plano(Punto(0,2,0), Direccion(0,-2,0), grisclaro, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO); //RGB({1,1,1}));
    Plano* bottomPlane   = new Plano(Punto(0,-2,0), Direccion(0,2,0), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);
    Plano* backPlane     = new Plano(Punto(0,0,2), Direccion(0,0,-2), RGB_GRIS, RGB_NEGRO, RGB_NEGRO, RGB_NEGRO);  

    // Color calido anaranjado
    RGB calido = RGB({1, 0.4, 0});
    ConoFinito* leftCone = new ConoFinito(Punto(0, 1.75, 0), Direccion(0,-1,0), M_PI/20, 3.75, RGB_GRIS, RGB_NEGRO, RGB_NEGRO, calido);
    Esfera* Sphere1  = new Esfera(Punto(1.5,-0.7,-1.25), 0.3, RGB_VERDE*0.3, RGB_GRIS*0.1, RGB_BLANCO*0.6, RGB_NEGRO, 1.5, N_AIRE); 
    Esfera* Sphere2  = new Esfera(Punto(-1.5,-0.7,1.25), 0.3, RGB_VERDE*0.3, RGB_GRIS*0.1, RGB_BLANCO*0.6, RGB_NEGRO, 1.5, N_AIRE); 
    Esfera* Sphere3  = new Esfera(Punto(1.5,-0.7,1.25), 0.3, RGB_VERDE*0.3, RGB_GRIS*0.1, RGB_BLANCO*0.6, RGB_NEGRO, 1.5, N_AIRE); 
    Esfera* Sphere4  = new Esfera(Punto(-1.5,-0.7,-1.25), 0.3, RGB_VERDE*0.3, RGB_GRIS*0.1, RGB_BLANCO*0.6, RGB_NEGRO, 1.5, N_AIRE); 

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    escena.anyadir(leftCone);
    escena.anyadir(Sphere1);
    escena.anyadir(Sphere2);
    escena.anyadir(Sphere3);
    escena.anyadir(Sphere4);

    Esfera* bola = new Esfera(Punto(0.22,0.7,-0.18), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.24,0.43,-0.25), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.77,-1.28,0.25), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.63,-1.5,-0.59), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.66,-0.73,0.05), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.14,-0.36,0.55), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.85,-1.58,-0.26), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.05,1.07,0.17), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.12,1.27,-0.04), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.48,-0.26,0.24), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.26,-0.86,0.64), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.03,-0.5,-0.6), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.93,-1.74,-0.07), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0,-1.15,-0.77), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0,-0.77,0), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.36,0.32,0.13), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.46,-1.6,0.77), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.29,-1.5,0.82), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.27,-1.08,0.7), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.69,-1.74,0.63), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.29,0.32,0.25), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.53,-0.24,0.06), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.64,-0.71,0.17), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.78,-1.17,0), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.41,-1.69,-0.82), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.52,-1,-0.52), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.5,-0.26,-0.2), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.28,0.09,-0.34), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.19,0.88,-0.13), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.58,-1.23,0.54), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(0.87,-1.63,0.25), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.36,-0.22,-0.38), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);
    bola = new Esfera(Punto(-0.49,-0.83,-0.49), 0.2, RGB_NEGRO, RGB_NEGRO, RGB_GRIS, calido, 1.5, N_AIRE);
    escena.anyadir(bola);

    escena.establecerCajaEnvolvente(Punto(-2,-2,-2), Punto(2,2,2));
    return escena;
}

Escena Ajedrez(){
    // Tablero
    Imagen* textura     = new Imagen("texturas/tablero.ppm");

    Plano* leftPlane    = new Plano(Punto(-0.5,0,0), Direccion(1,0,0), textura); 
    Plano* rightPlane   = new Plano(Punto(0.5,0,0), Direccion(-1,0,0), textura); 
    Plano* topPlane     = new Plano(Punto(0,0.5,0), Direccion(0,-1,0), textura); 
    Plano* bottomPlane  = new Plano(Punto(0,-0.5,0), Direccion(0,1,0), textura); 
    Plano* backPlane    = new Plano(Punto(0,0,0.5), Direccion(0,0,-1), textura); 

    Escena escena = Escena();
    escena.anyadir(leftPlane);
    escena.anyadir(rightPlane);
    escena.anyadir(topPlane);
    escena.anyadir(bottomPlane);
    escena.anyadir(backPlane);
    
    //-------------//
    // bottomPlane //
    //-------------//
    // Peones
    Esfera* bola = new Esfera(Punto(0.0625,-0.4,0.0625), 0.02, RGB_NEGRO, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    ConoFinito* leftCone = new ConoFinito(Punto(0.0625, -0.4, 0.0625), Direccion(0,-1,0), M_PI/20, 0.1, RGB_NEGRO, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    bola = new Esfera(Punto(-0.0625,-0.4,-0.0625), 0.02, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    leftCone = new ConoFinito(Punto(-0.0625, -0.4, -0.0625), Direccion(0,-1,0), M_PI/20, 0.1, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    // Torre
    CilindroFinito* cilindroB = new CilindroFinito(Punto(-0.1925,-0.38,-0.3225), Direccion(0,-1,0), 0.03125, 0.12, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    CilindroFinito* cilindroA = new CilindroFinito(Punto(-0.1925,-0.38,-0.3225), Direccion(0,1,0), 0.05, 0.03, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(cilindroB);
    escena.anyadir(cilindroA);
    // Alfil
    leftCone = new ConoFinito(Punto(0.1925,-0.38,-0.1925), Direccion(0,-1,0), M_PI/20, 0.12, RGB_NEGRO, RGB_GRIS,RGB_NEGRO, RGB_NEGRO);
    cilindroA = new CilindroFinito(Punto(0.1925,-0.38,-0.1925), Direccion(0,1,0), 0.025, 0.01, RGB_NEGRO, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    bola = new Esfera(Punto(0.1925,-0.36,-0.1925), 0.02, RGB_NEGRO, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(leftCone);
    escena.anyadir(cilindroA);
    escena.anyadir(bola);

    //-----------//
    // backPlane //
    //-----------//
    // Peones
    bola = new Esfera(Punto(0.0625,0.0625,0.4), 0.02, RGB_MAGENTA*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    leftCone = new ConoFinito(Punto(0.0625, 0.0625, 0.4), Direccion(0,0,1), M_PI/20, 0.1, RGB_MAGENTA*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    bola = new Esfera(Punto(-0.0625,-0.0625,0.4), 0.02, RGB_VERDE*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    leftCone = new ConoFinito(Punto(-0.0625, -0.0625, 0.4), Direccion(0,0,1), M_PI/20, 0.1, RGB_VERDE*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    // Torre
    cilindroB = new CilindroFinito(Punto(-0.1925,-0.3225,0.38), Direccion(0,0,1), 0.03125, 0.12, RGB_VERDE*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    cilindroA = new CilindroFinito(Punto(-0.1925,-0.3225,0.38), Direccion(0,0,-1), 0.05, 0.03, RGB_VERDE*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(cilindroB);
    escena.anyadir(cilindroA);
    // Alfil
    leftCone = new ConoFinito(Punto(0.1925,-0.3225,0.38), Direccion(0,0,1), M_PI/20, 0.12, RGB_MAGENTA*0.9, RGB_GRIS,RGB_NEGRO, RGB_NEGRO);
    cilindroA = new CilindroFinito(Punto(0.1925,-0.3225,0.38), Direccion(0,0,-1), 0.025, 0.01, RGB_MAGENTA*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    bola = new Esfera(Punto(0.1925,-0.3225,0.36), 0.02, RGB_MAGENTA*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(leftCone);
    escena.anyadir(cilindroA);
    escena.anyadir(bola);

    //-------------//
    // topPlane    //
    //-------------//
    // Peones
    bola = new Esfera(Punto(0.0625,0.4,0.0625), 0.02, RGB_NEGRO,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    leftCone = new ConoFinito(Punto(0.0625, 0.4, 0.0625), Direccion(0,1,0), M_PI/20, 0.1, RGB_NEGRO,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    bola = new Esfera(Punto(-0.0625,0.4,-0.0625), 0.02, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    leftCone = new ConoFinito(Punto(-0.0625, 0.4, -0.0625), Direccion(0,1,0), M_PI/20, 0.1, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    // Torre
    cilindroB = new CilindroFinito(Punto(-0.1925,0.38,-0.3225), Direccion(0,1,0), 0.03125, 0.12, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    cilindroA = new CilindroFinito(Punto(-0.1925,0.38,-0.3225), Direccion(0,-1,0), 0.05, 0.03, RGB_BLANCO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(cilindroB);
    escena.anyadir(cilindroA);
    // Alfil
    leftCone = new ConoFinito(Punto(0.1925,0.38,-0.3225), Direccion(0,1,0), M_PI/20, 0.12, RGB_NEGRO,RGB_GRIS*0.2,RGB_NEGRO, RGB_NEGRO);
    cilindroA = new CilindroFinito(Punto(0.1925,0.38,-0.3225), Direccion(0,-1,0), 0.025, 0.01, RGB_NEGRO,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    bola = new Esfera(Punto(0.1925,0.36,-0.3225), 0.02, RGB_NEGRO,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(leftCone);
    escena.anyadir(cilindroA);
    escena.anyadir(bola);

    //-------------//
    // rightPlane  //
    //-------------//
    // Peones
    bola = new Esfera(Punto(0.4,0.0625,0.0625), 0.02, RGB_ROJO*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    leftCone = new ConoFinito(Punto(0.4,0.0625, 0.0625), Direccion(1,0,0), M_PI/20, 0.1, RGB_ROJO*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    bola = new Esfera(Punto(0.4,-0.0625,-0.0625), 0.02, RGB_CIAN2*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    leftCone = new ConoFinito(Punto( 0.4, -0.0625,-0.0625), Direccion(1,0,0), M_PI/20, 0.1, RGB_CIAN2*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    // Torre
    cilindroB = new CilindroFinito(Punto(0.38,-0.1925,-0.3225), Direccion(1,0,0), 0.03125, 0.12, RGB_CIAN2*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    cilindroA = new CilindroFinito(Punto(0.38,-0.1925,-0.3225), Direccion(-1,0,0), 0.05, 0.03, RGB_CIAN2*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(cilindroB);
    escena.anyadir(cilindroA);
    // Alfil
    leftCone = new ConoFinito(Punto(0.38,0.1925,-0.1925), Direccion(1,0,0), M_PI/20, 0.12, RGB_ROJO*0.9,RGB_GRIS*0.2,RGB_NEGRO, RGB_NEGRO);
    cilindroA = new CilindroFinito(Punto(0.38,0.1925,-0.1925), Direccion(-1,0,0), 0.025, 0.01, RGB_ROJO*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    bola = new Esfera(Punto(0.36,0.1925,-0.1925), 0.02, RGB_ROJO*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(leftCone);
    escena.anyadir(cilindroA);
    escena.anyadir(bola);

    //-------------//
    // leftPlane   //
    //-------------//
    // Peones
    bola = new Esfera(Punto(-0.4,0.0625,0.0625), 0.02, RGB_AZUL*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    leftCone = new ConoFinito(Punto(-0.4,0.0625, 0.0625), Direccion(-1,0,0), M_PI/20, 0.1, RGB_AZUL*0.9,RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    bola = new Esfera(Punto(-0.4,-0.0625,-0.0625), 0.02, RGB_AMARILLO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    leftCone = new ConoFinito(Punto( -0.4, -0.0625,-0.0625), Direccion(-1,0,0), M_PI/20, 0.1, RGB_AMARILLO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(bola);
    escena.anyadir(leftCone);
    // Torre
    cilindroB = new CilindroFinito(Punto(-0.38,-0.1925,-0.3225), Direccion(-1,0,0), 0.03125, 0.12, RGB_AMARILLO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    cilindroA = new CilindroFinito(Punto(-0.38,-0.1925,-0.3225), Direccion(1,0,0), 0.05, 0.03, RGB_AMARILLO*0.2, RGB_BLANCO*0.05, RGB_BLANCO*0.75, RGB_NEGRO, 1.1, N_AIRE);
    escena.anyadir(cilindroB);
    escena.anyadir(cilindroA);
    // Alfil
    leftCone = new ConoFinito(Punto(-0.38,0.1925,-0.3225), Direccion(-1,0,0), M_PI/20, 0.12, RGB_AZUL*0.9, RGB_GRIS*0.2,RGB_NEGRO, RGB_NEGRO);
    cilindroA = new CilindroFinito(Punto(-0.38,0.1925,-0.3225), Direccion(1,0,0), 0.025, 0.01, RGB_AZUL*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    bola = new Esfera(Punto(-0.36,0.1925,-0.3225), 0.02, RGB_AZUL*0.9, RGB_GRIS*0.2, RGB_NEGRO, RGB_NEGRO);
    escena.anyadir(leftCone);
    escena.anyadir(cilindroA);
    escena.anyadir(bola);

    LuzPuntual* luz = new LuzPuntual(Punto(0,0,-0.5), RGB_BLANCO);
    escena.anyadirLuz(luz);
    escena.establecerCajaEnvolvente(Punto(-0.55,-0.55,-0.55), Punto(0.55,0.55,0.55));
    return escena;
}