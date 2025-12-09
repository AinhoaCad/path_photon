// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>
#include <time.h>
#include <algorithm>
#include "geometria.h"
#include "aleatorio.h"

using namespace std;

std::ostream & operator<<( std::ostream &os, const Interseccion &i ) {
    os  << endl << fixed << setprecision(2)
        << "Interseccion: " << i.numIntersecciones << endl
        << "Distancia: " << i.distancia << endl
        << "Punto: " << i.punto << endl
        << "Normal: " << i.normal << endl
        << setprecision(0);     

    return os;
}

ostream& operator<<(ostream& os, const EVENTO& e) {
    switch (e) {
        case EVENTO_ABSORCION:
            os << "Absorción";
            break;
        case EVENTO_EMISION:
            os << "Emisión";
            break;
        case EVENTO_DIFUSO:
            os << "Difuso";
            break;
        case EVENTO_ESPECULAR:
            os << "Especular";
            break;
        case EVENTO_REFRACCION:
            os << "Refracción";
            break;
        default:
            os << "Evento desconocido";
            break;
    }
    return os;
}

// ---------------------------- Geometria ----------------------------
Geometria::Geometria(RGB k[4], double n_d, double n_f) : k_d(k[0]), 
            k_s(k[1]), k_t(k[2]), k_e(k[3]), n_dentro(n_d), n_fuera(n_f) {
    p_d = max(k_d);
    p_s = max(k_s);
    p_t = max(k_t);
    p_e = max(k_e);
    // normalizamos para asegurar un 10% de probabilidad de absorción
    double p_total = p_e + p_d + p_s + p_t;
    p_e = 0.9 * p_e / p_total;
    p_d = 0.9 * p_d / p_total;
    p_s = 0.9 * p_s / p_total;
    p_t = 0.9 * p_t / p_total;
    textura = NULL;
};

Geometria::Geometria(RGB kd, RGB ks, RGB kt, RGB ke, double n_d, double n_f) : 
            k_d(kd), k_s(ks), k_t(kt), k_e(ke), n_dentro(n_d), n_fuera(n_f) {
    p_d = max(k_d);
    p_s = max(k_s);
    p_t = max(k_t);
    p_e = max(k_e);
    // normalizamos para asegurar un 10% de probabilidad de absorción
    double p_total = p_e + p_d + p_s + p_t;
    if (p_total != 0) {
        p_e = 0.9 * p_e / p_total;
        p_d = 0.9 * p_d / p_total;
        p_s = 0.9 * p_s / p_total;
        p_t = 0.9 * p_t / p_total;
    }
    textura = NULL;
};

Geometria::Geometria(Imagen* img, double n_d, double n_f) 
            : n_dentro(n_d), n_fuera(n_f) {
    k_d = RGB({1,1,1});
    k_s = RGB({0,0,0});
    k_t = RGB({0,0,0});
    k_e = RGB({0,0,0});
    p_d = 1;
    p_s = 0;
    p_t = 0;
    p_e = 0;
    // normalizamos para asegurar un 10% de probabilidad de absorción
    double p_total = p_e + p_d + p_s + p_t;
    if (p_total != 0) {
        p_e = 0.9 * p_e / p_total;
        p_d = 0.9 * p_d / p_total;
        p_s = 0.9 * p_s / p_total;
        p_t = 0.9 * p_t / p_total;
    }
    textura = img;
};

EVENTO Geometria::evento_aleatorio() {
    double p = (double) rand() / RAND_MAX;
    if (p <= p_e) {
        return EVENTO_EMISION;
    } else if (p <= p_e + p_d) {
        return EVENTO_DIFUSO;
    } else if (p <= p_e + p_d + p_s) {
        return EVENTO_ESPECULAR;
    } else if (p <= p_e + p_d + p_s + p_t) {
        return EVENTO_REFRACCION;
    } else {
        return EVENTO_ABSORCION;
    }
}

RGB Geometria::fr_solo_difuso(Punto x, Direccion wi, Direccion wo, Direccion n) {
    return k_d_(x)/M_PI;
}

RGB Geometria::fr_difuso(Punto x, Direccion wi, Direccion wo, Direccion n) {
    return k_d_(x)/p_d;
}

RGB Geometria::fr_especular(Punto x, Direccion wi, Direccion wo, Direccion n) {
    return k_s/p_s;
}

RGB Geometria::fr_refraccion(Punto x, Direccion wi, Direccion wo, Direccion n) {
    return k_t/p_t;
}

RGB Geometria::fr(Punto x, Direccion wi, Direccion wo, Direccion n) {
    RGB rgb = RGB({0,0,0});
    Direccion wr = reflejar(wo, n);
    Direccion wt = refractar(wo, n);
    if (wi == wr) { // especular
        rgb = k_s/p_s;
    } else if (wi == wt) { // refracción
        rgb = k_t/p_t;
    } else { // difuso
        rgb =  k_d_(x)/p_d;
    }
    return rgb;
}

Direccion Geometria::reflejar(Direccion wi, Direccion n) {
    return wi - 2*(wi*n)*n;
}

Direccion Geometria::refractar(Direccion wi, Direccion n) {
    float eta = n_fuera / n_dentro;
    float cosI = n.normalizado()*wi.normalizado();
    Direccion normal = n.normalizado();
    if (cosI < 0) {
        cosI = -cosI;
    } else {
        normal = -normal;
        eta = 1 / eta;
    }
    double k = 1 - eta * eta * (1.0 - cosI*cosI);
    if (k < 0) {
        return Direccion();
    } else {
        return eta*wi.normalizado() + (eta * cosI - sqrt(k))*normal;
    }
}

// ------------------- Intersecciones ---------------------
Interseccion Esfera::interseccion(Rayo r) { 
    double a = r.d*r.d;
    double b = 2*(r.p-centro)*r.d;
    double c = ((r.p-centro)*(r.p-centro))-(radio*radio);
    double discriminante =  b*b-4*a*c;
    if (discriminante < 0) return Interseccion();
    else if (discriminante > 0) {
        double t1 = (-b - sqrt(discriminante))/(2*a);
        double t2 = (-b + sqrt(discriminante))/(2*a);
        double t;
        if (0 < t1) {    
            t = t1;
        } else t = t2;
        Direccion normal = (r.p + t*r.d - centro).normalizado();
        return Interseccion(t, r.p + t*r.d, normal, this);
    } else { // discriminante == 0
        double t = -b/(2*a);
        Direccion normal = (r.p + t*r.d - centro).normalizado();
        return Interseccion(t, r.p + t*r.d, normal, this);
    }
}

Interseccion Plano::interseccion(Rayo r) {
    double discriminante = normal*r.d;
    if (discriminante == 0) return Interseccion();
    else {
        double t = ((this->p-r.p)*normal)/discriminante;
        return Interseccion(t, r.p + t*r.d, normal, this);
    }
}

Interseccion Triangulo::interseccion(Rayo r) {
    Direccion e1 = puntos[1] - puntos[0];
    Direccion e2 = puntos[2] - puntos[0];
    Direccion s = r.p - puntos[0];
    Direccion s1 = r.d.productoVectorial(e2);
    Direccion s2 = s.productoVectorial(e1);
    double divisor = s1*e1;
    if (divisor == 0) return Interseccion();
    else {
        double t = s2*e2/divisor;
        double alfa = s1*s/divisor;
        double beta = s2*r.d/divisor;
        if (alfa >= 0 && beta >= 0 && alfa + beta <= 1) {
            Direccion normal = e1.productoVectorial(e2).normalizado();
        return Interseccion(t, r.p + t*r.d, normal, this);
        } else return Interseccion();
    }
}

Interseccion Cono::interseccion(Rayo r) {
    Direccion co = r.p - centro;
    double cosa = cos(angulo);
    cosa = cosa*cosa;
    double sina = sin(angulo);
    sina = sina*sina;
    double a = cosa*(r.d*r.d) - (r.d*eje)*(r.d*eje) - sina*((r.d*eje)*(r.d*eje));
    double b = 2*(cosa*(r.d*co) - (r.d*eje)*(co*eje) - sina*((r.d*eje)*(co*eje)));
    double c = cosa*(co*co) - (co*eje)*(co*eje) - sina*((co*eje)*(co*eje));
    double discriminante =  b*b-4*a*c;
    if (discriminante < 0) return Interseccion();
    else if (discriminante > 0) {
        discriminante = sqrt(discriminante);
        double t1 = (-b - (discriminante))/(2*a);
        double t2 = (-b + (discriminante))/(2*a);
        double t = t1;
        if (t < 0) {
            t = t2;
        }
        if (t < 0) return Interseccion();
        Punto p = r.p + t*r.d;
        Direccion cp = p - centro;
        double h = cp*eje;
        if (h < 0 ) return Interseccion();
        Direccion tangente = cp.productoVectorial(eje).normalizado();
        Direccion normal = cp.productoVectorial(tangente).normalizado();
        return Interseccion(t, p, normal, this);
    } else { // discriminante == 0
        double t = -b/(2*a);
        Punto p = r.p + t*r.d;
        Direccion cp = p - centro;
        Direccion tangente = cp.productoVectorial(eje).normalizado();
        Direccion normal = cp.productoVectorial(tangente).normalizado();
        return Interseccion(t, p, normal, this);
    }
}

Interseccion ConoFinito::interseccion(Rayo r) {
    Direccion co = r.p - centro;
    double cosa = cos(angulo);
    cosa = cosa*cosa;
    double sina = sin(angulo);
    sina = sina*sina;
    double a = cosa*(r.d*r.d) - (r.d*eje)*(r.d*eje) - sina*((r.d*eje)*(r.d*eje));
    double b = 2*(cosa*(r.d*co) - (r.d*eje)*(co*eje) - sina*((r.d*eje)*(co*eje)));
    double c = cosa*(co*co) - (co*eje)*(co*eje) - sina*((co*eje)*(co*eje));
    double discriminante =  b*b-4*a*c;
    if (discriminante < 0) return Interseccion();
    else if (discriminante > 0) {
        discriminante = sqrt(discriminante);
        double t1 = (-b - (discriminante))/(2*a);
        double t2 = (-b + (discriminante))/(2*a);
        double t = t1;
        if (t < 0) {
            t = t2;
        }
        if (t < 0) return Interseccion();
        Punto p = r.p + t*r.d;
        Direccion cp = p - centro;
        double h = cp*eje;
        if (h < 0 || h > altura) return Interseccion();
        Direccion tangente = cp.productoVectorial(eje).normalizado();
        Direccion normal = cp.productoVectorial(tangente).normalizado();
        return Interseccion(t, p, normal, this);
    } else { // discriminante == 0
        double t = -b/(2*a);
        Punto p = r.p + t*r.d;
        Direccion cp = p - centro;
        double h = cp*eje;
        if (h < 0 || h > altura) return Interseccion();
        Direccion tangente = cp.productoVectorial(eje).normalizado();
        Direccion normal = cp.productoVectorial(tangente).normalizado();
        return Interseccion(t, p, normal, this);
    }
}

Interseccion Cilindro::interseccion(Rayo r) {
    Direccion co = r.p - centro;
    double a = (r.d*r.d) - (r.d*eje)*(r.d*eje);
    double b = 2*(r.d*co - (r.d*eje)*(co*eje));
    double c = co*co - (co*eje)*(co*eje) - radio*radio;
    double discriminante =  b*b-4*a*c;
    if (discriminante < 0) return Interseccion();
    else if (discriminante > 0) {
        double t1 = (-b - sqrt(discriminante))/(2*a);
        double t2 = (-b + sqrt(discriminante))/(2*a);
        double t;
        if (0 < t1 && t1 <= t2) {
            t = t1;
        } else t = t2;
        Punto p = r.p + t1*r.d;
        Direccion normal = (p - centro - (p - centro)*eje*eje).normalizado();
        return Interseccion(t, p, normal, this);
    } else { // discriminante == 0
        double t = -b/(2*a);
        Punto p = r.p + t*r.d;
        Direccion normal = (p - centro - (p - centro)*eje*eje).normalizado();
        return Interseccion(t, p, normal, this);
    }
}

Interseccion CilindroFinito::interseccion(Rayo r) {
    Direccion co = r.p - centro;
    double a = (r.d*r.d) - (r.d*eje)*(r.d*eje);
    double b = 2*(r.d*co - (r.d*eje)*(co*eje));
    double c = co*co - (co*eje)*(co*eje) - radio*radio;
    double discriminante =  b*b-4*a*c;
    if (discriminante < 0) return Interseccion();
    else if (discriminante > 0) {
        double t1 = (-b - sqrt(discriminante))/(2*a);
        double t2 = (-b + sqrt(discriminante))/(2*a);
        double t = t1;
        Punto p = r.p + t*r.d;
        double distancia = (p - centro)*eje;
        if (distancia < 0 || distancia > altura || t < 0) {
            t = t2;
            p = r.p + t*r.d;
        }
        distancia = (p - centro)*eje;
        if (distancia < 0 || distancia > altura) {
            return Interseccion();
        }
        Direccion normal = (p - centro - (p - centro)*eje*eje).normalizado();
        return Interseccion(t, p, normal, this);
    } else { // discriminante == 0
        double t = -b/(2*a);
        Punto p = r.p + t*r.d;
        double distancia = (p - centro)*eje;
        if (distancia < 0 || distancia > altura) {
            return Interseccion();
        }
        Direccion normal = (p - centro - (p - centro)*eje*eje).normalizado();
        return Interseccion(t, p, normal, this);
    }
}

Interseccion Disco::interseccion(Rayo r) {
    double discriminante = normal*r.d;
    if (discriminante == 0) return Interseccion();
    else {
        double t = ((this->centro-r.p)*normal)/discriminante;
        Punto p = r.p + t*r.d;
        double distancia = (p - centro).modulo();
        if (distancia > radio) return Interseccion();
        return Interseccion(t, p, normal, this);
    }
}

Interseccion Caja::interseccion(Rayo r) {
    double tmin = -INFINITY;
    double tmax = INFINITY;
    int eje = -1;
    for (int i = 0; i < 3; i++) {
        double t1 = (pmin.h[i] - r.p.h[i])/r.d.h[i];
        double t2 = (pmax.h[i] - r.p.h[i])/r.d.h[i];
        if (t1 > t2) {
            swap(t1, t2);
        }
        if (t1 > tmin) {tmin = t1; eje = i;}
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return Interseccion();
        if (tmax < 0) return Interseccion();
    }
    if (tmin == -INFINITY) return Interseccion();
    Punto p = r.p + tmin*r.d;
    Direccion normal;
    switch (eje) {
        case 0:
            if (r.d.h[0] < 0) normal = Direccion(1,0,0);
            else normal = Direccion(-1,0,0);
            break;
        case 1:
            if (r.d.h[1] < 0) normal = Direccion(0,1,0);
            else normal = Direccion(0,-1,0);
            break;
        case 2:
            if (r.d.h[2] < 0) normal = Direccion(0,0,1);
            else normal = Direccion(0,0,-1);
            break;
        default:
            break;
    }
    return Interseccion(tmin, p, normal, this);
}

// ------------------- Cajas envolventes ---------------------
Punto Triangulo::cajaEnvolventeMin() {
    Punto pmin = puntos[0];
    for (int i = 1; i < 3; i++) {
        if (puntos[i].h[0] < pmin.h[0]) pmin.h[0] = puntos[i].h[0];
        if (puntos[i].h[1] < pmin.h[1]) pmin.h[1] = puntos[i].h[1];
        if (puntos[i].h[2] < pmin.h[2]) pmin.h[2] = puntos[i].h[2];
    }
    return pmin;
}

Punto Triangulo::cajaEnvolventeMax() {
    Punto pmax = puntos[0];
    for (int i = 1; i < 3; i++) {
        if (puntos[i].h[0] > pmax.h[0]) pmax.h[0] = puntos[i].h[0];
        if (puntos[i].h[1] > pmax.h[1]) pmax.h[1] = puntos[i].h[1];
        if (puntos[i].h[2] > pmax.h[2]) pmax.h[2] = puntos[i].h[2];
    }
    return pmax;
}

// ------------------- RENDER ESPECTRAL ---------------------
// En lugar de tener propiedades de emisión RGB para cada objeto,
// incluye una representación espectral del color que luego se convierte a RGB utilizando las
// curvas estándar de RGB de CIE. La representación espectral puede ser un vector de 8, 16 o
// 32 valores, o incluso una función espectral (una función que depende de la longitud de onda)
// que se muestrea en los valores correspondientes de los 8, 16 o 32 canales.

Espectral::Espectral(double v[32], int n){
    this->numValores = n;
    for (int i = 0; i < n; i++) {
        this->valores[i] = v[i];
    }
}

Espectral::Espectral(RGB rgb){
    // TO DO
}
