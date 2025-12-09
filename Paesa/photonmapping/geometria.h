// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include "coordenadas.h"
#include "imagenes.h"
// incluimos random para generar numeros aleatorios
#include <random>

#define N_AIRE 1.000293

using namespace std;

// ---------------------------- Rayo ----------------------------
class Rayo {
    public:
    Punto p;
    Direccion d;
    // Constructor punto y direccion
    Rayo(Punto p, Direccion d) : p(p), d(d){};
};

class Geometria;
// ---------------------------- Interseccion ----------------------------
class Interseccion {
    public:
    int numIntersecciones;
    double distancia;
    Punto punto;
    Direccion normal;
    Geometria* geometria;
    
    Interseccion() : numIntersecciones(0), distancia(0), punto(Punto({0,0,0})), 
        normal(Direccion({0,0,0})), geometria(NULL) {};

    Interseccion(double t, Punto p, Direccion n, Geometria* g, bool haciaDentro = true) : numIntersecciones(1), 
        distancia(t), punto(p), normal(n), geometria(g) {};
};

ostream& operator<<(ostream& os, const Interseccion& i);

enum EVENTO { // Eventos de interacción de la luz con la geometría
    EVENTO_EMISION = 0,
    EVENTO_DIFUSO = 1,
    EVENTO_ESPECULAR = 2,
    EVENTO_REFRACCION = 3,
    EVENTO_ABSORCION = 4
};

ostream& operator<<(ostream& os, const EVENTO& e);

// ---------------------------- Geometria ----------------------------
class Geometria {
    public:
    RGB k_d, k_s, k_t, k_e;
    double p_d, p_s, p_t, p_e;
    double n_dentro, n_fuera;
    Imagen* textura;

    // Constructor
    Geometria(RGB k[4], double n_d = N_AIRE, double n_f = N_AIRE);

    Geometria(RGB kd, RGB ks, RGB kt, RGB ke, double n_d = N_AIRE, double n_f = N_AIRE);

    // Constructor en base a la textura de una imagen
    Geometria(Imagen* img, double n_d = N_AIRE, double n_f = N_AIRE);

    // Todas las geometrias deben implementar la interseccion con un rayo
    virtual Interseccion interseccion(Rayo r) = 0; 

    virtual Punto cajaEnvolventeMin() = 0;
    virtual Punto cajaEnvolventeMax() = 0;

    virtual double obtener_u_textura(Punto x) {
        // No implementado
        return 0;
    };

    virtual double obtener_v_textura(Punto x) {
        // No implementado
        return 0;
    };

    EVENTO evento_aleatorio();

    RGB k_d_(Punto x) { 
        if (textura != NULL) {
            int x_ = floor(textura->ancho*obtener_u_textura(x)) + 1;
            int y_ = floor(textura->alto*obtener_v_textura(x)) + 1;
            return textura->obtenerPixel(x_, y_);
        }
        return k_d; };

    RGB fr_solo_difuso(Punto x, Direccion wi, Direccion wo, Direccion n);
    RGB fr_difuso(Punto x, Direccion wi, Direccion wo, Direccion n);
    RGB fr_especular(Punto x, Direccion wi, Direccion wo, Direccion n);
    RGB fr_refraccion(Punto x, Direccion wi, Direccion wo, Direccion n);

    RGB fr(Punto x, Direccion wi, Direccion wo, Direccion n);

    Direccion reflejar(Direccion wi, Direccion n);
    Direccion refractar(Direccion wi, Direccion n);

    bool esLuzArea() {
        return max(k_e) > 0 && max(k_d) == 0 && max(k_s) == 0 && max(k_t) == 0;
    };

};

// ---------------------------- Esfera ----------------------------
class Esfera : public Geometria {
    public:
    Punto centro;
    double radio;
    
    // Constructor centro y radio
    Esfera(Punto c, double r, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), radio(r) {};

    Esfera(Punto c, double r, Imagen* img, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(img, n_dentro, n_fuera), centro(c), radio(r) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(centro.h[0]-radio, centro.h[1]-radio, centro.h[2]-radio);
    };
    Punto cajaEnvolventeMax() {
        return Punto(centro.h[0]+radio, centro.h[1]+radio, centro.h[2]+radio);
    };

    double obtener_u_textura(Punto x) override {
        Direccion n = (x - centro).normalizado();
        return 0.5 - asin(n.h[1]) / M_PI;
    };
    double obtener_v_textura(Punto x) override {
        Direccion n = (x - centro).normalizado();
        return 0.5 + atan2(n.h[2], n.h[0]) / (2 * M_PI);
    };
};

// ---------------------------- Plano ----------------------------
class Plano : public Geometria {
    public:
    Punto p;
    Direccion normal, u, v;

    // Constructor punto y normal
    Plano(Punto p, Direccion normal,RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), p(p), normal(normal) {
                u = normal.productoVectorial(Direccion({0,0,1}));
                if (u.modulo() == 0) {
                    u = normal.productoVectorial(Direccion({1,0,0}));
                }
                v = normal.productoVectorial(u).normalizado();
                u = v.productoVectorial(normal);
            };

    Plano(Punto p, Direccion normal, Imagen* img, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(img, n_dentro, n_fuera), p(p), normal(normal) {
                u = normal.productoVectorial(Direccion({0,0,1}));
                if (u.modulo() == 0) {
                    u = normal.productoVectorial(Direccion({1,0,0}));
                }
                v = normal.productoVectorial(u).normalizado();
                u = v.productoVectorial(normal);
            };

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);};

    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);};

    double obtener_u_textura(Punto x) override {
        double u_ = (x - p)*(this->u);
        double u_textura = u_ - (int)u_;
        return (u_textura < 0) ? u_textura + 1 : u_textura;
    };

    double obtener_v_textura(Punto x) override {
        double v_ = (x - p)*(this->v);
        double v_textura = v_ - (int)v_;
        return (v_textura < 0) ? v_textura + 1 : v_textura;
    };
};

// ---------------------------- Triangulo ----------------------------
class Triangulo : public Geometria {
    public:
    Punto puntos[3];

    // Constructor puntos
    Triangulo(Punto p0, Punto p1, Punto p2, RGB k[4]) : Geometria(k) {
        puntos[0] = p0; puntos[1] = p1; puntos[2] = p2;
    };
    Triangulo(Punto p0, Punto p1, Punto p2, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera) { puntos[0] = p0; puntos[1] = p1; puntos[2] = p2; };

    Interseccion interseccion(Rayo r);
    Punto cajaEnvolventeMin();
    Punto cajaEnvolventeMax();
};

// ---------------------------- Cono ----------------------------
class Cono : public Geometria {
    public:
    Punto centro; // Punto del eje
    Direccion eje; // eje normalizado
    double angulo; // angulo del cono

    // Constructor centro, eje y angulo
    Cono(Punto c, Direccion e, double th, RGB k[4]) : Geometria(k), centro(c), eje(e), angulo(th) {};
    Cono(Punto c, Direccion e, double th, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), eje(e), angulo(th) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);
    };
    
    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);
    };

};

// ---------------------------- ConoFinito ----------------------------
class ConoFinito : public Geometria {
    public:
    Punto centro; // Punto del eje
    Direccion eje; // eje normalizado
    double angulo; // angulo del cono
    double altura; // altura del cono

    // Constructor centro, eje, angulo y altura
    ConoFinito(Punto c, Direccion e, double th, double h, RGB k[4]) : Geometria(k), centro(c), eje(e), angulo(th), altura(h) {};
    ConoFinito(Punto c, Direccion e, double th, double h, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), eje(e), angulo(th), altura(h) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);
    };
    
    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);
    };

};

// ---------------------------- Cilindro ----------------------------
class Cilindro : public Geometria {
    public:
    Punto centro; // Punto del eje
    Direccion eje; // eje normalizado
    double radio; // radio del cilindro

    // Constructor centro, eje y radio
    Cilindro(Punto c, Direccion e, double r, RGB k[4]) : Geometria(k), centro(c), eje(e), radio(r) {};
    Cilindro(Punto c, Direccion e, double r, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), eje(e), radio(r) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);
    };
    
    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);
    };
};

// ---------------------------- CilindroFinito ----------------------------
class CilindroFinito : public Geometria {
    public:
    Punto centro; // Punto del eje
    Direccion eje; // eje normalizado
    double radio; // radio del cilindro
    double altura; // altura del cilindro

    // Constructor centro, eje, radio y altura
    CilindroFinito(Punto c, Direccion e, double r, double h, RGB k[4]) : Geometria(k), centro(c), eje(e), radio(r), altura(h) {};
    CilindroFinito(Punto c, Direccion e, double r, double h, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), eje(e), radio(r), altura(h) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);};
        
    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);};
};

// ---------------------------- Disco ----------------------------
class Disco : public Geometria {
    public:
    Punto centro; // Punto del eje
    Direccion normal; // eje normalizado
    double radio; // radio del disco

    // Constructor centro, normal y radio
    Disco(Punto c, Direccion n, double r, RGB k[4]) : Geometria(k), centro(c), normal(n), radio(r) {};
    Disco(Punto c, Direccion n, double r, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), centro(c), normal(n), radio(r) {};

    Interseccion interseccion(Rayo r);

    Punto cajaEnvolventeMin() {
        return Punto(INFINITY,INFINITY,INFINITY);};
        
    Punto cajaEnvolventeMax() {
        return Punto(-INFINITY,-INFINITY,-INFINITY);};
};

// ---------------------------- Caja ----------------------------
class Caja : public Geometria {
    public:
    Punto pmin, pmax; // Esquinas aliadas con los ejes

    // Constructor puntos
    Caja(Punto p0, Punto p1, RGB k[4]) : Geometria(k), pmin(p0), pmax(p1) {};
    Caja(Punto p0, Punto p1, RGB kd, RGB ks, RGB kt, RGB ke, double n_dentro = N_AIRE, double n_fuera = N_AIRE) : 
            Geometria(kd, ks, kt, ke, n_dentro, n_fuera), pmin(p0), pmax(p1) {};

    Caja(Punto p0, Punto p1) : Geometria(RGB({0,0,0}), 
        RGB({0,0,0}), RGB({0,0,0}), RGB({0,0,0})), pmin(p0), pmax(p1) {};

    Interseccion interseccion(Rayo r);
    
    Punto cajaEnvolventeMin() {
        return pmin;
    };

    Punto cajaEnvolventeMax() {
        return pmax;
    };
};

class LuzPuntual { 
    public:
    Punto p;
    RGB c;

    // Constructor
    LuzPuntual(Punto p, RGB c) : p(p), c(c){};
};

// ------------------- RENDER ESPECTRAL ---------------------
// En lugar de tener propiedades de emisión RGB para cada objeto,
// incluye una representación espectral del color que luego se convierte a RGB utilizando las
// curvas estándar de RGB de CIE. La representación espectral puede ser un vector de 8, 16 o
// 32 valores, o incluso una función espectral (una función que depende de la longitud de onda)
// que se muestrea en los valores correspondientes de los 8, 16 o 32 canales.
class Espectral {
    public:
    double valores[32];
    int numValores;

    // Constructor vectorial
    Espectral(double v[32], int n);

    // Constructor RGB
    Espectral(RGB rgb);

    RGB convertirRGB();
};


#endif // GEOMETRIA_H