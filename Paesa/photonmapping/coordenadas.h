// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#ifndef COORDENADAS_H
#define COORDENADAS_H

#include <iostream>

using namespace std;

#ifndef M_PI
    #define M_PI 3.14159265358979323846 // Pi
#endif

// ------------------------ COORD HOMOGENEAS -------------------------
class CoordH {
    public:
    double h[4];
    // Constructor por defecto
    CoordH() : h{0,0,0,0} {};
    // Constructor de 4 componentes
    CoordH(double x, double y, double z, double h) : h{x,y,z,h} {};
    // Constructor vector
    CoordH(double v[4]) : h{v[0],v[1],v[2],v[3]} {};
};

ostream& operator<<(ostream& os, const CoordH& c);

class Direccion;

// ----------------------------- PUNTO -----------------------------
class Punto: public CoordH {
    public:
    // Constructor por defecto
    Punto() : CoordH(0,0,0,1) {};
    // Constructor
    Punto(double x, double y, double z) : CoordH(x,y,z,1) {};
    // Constructor vectorial
    Punto(double c[3]) : CoordH(c[0],c[1],c[2],1) {};

    // OPERADORES
    Punto operator+(Direccion d);
    Punto operator-(Direccion d);
    Direccion operator-(Punto p);
    Punto operator*(double k);
    Punto operator/(double k);
    Punto operator-();

    bool operator==(Punto p);
    bool operator!=(Punto p);
    const double& operator[](size_t i) const;
};

ostream& operator<<(ostream& os, const Punto& p);
Punto operator*(double k, Punto p);

// ----------------------------- DIRECCION -----------------------------
class Direccion: public CoordH {
    public:
    // Constructor por defecto
    Direccion() : CoordH(0,0,0,0) {};
    // Constructor
    Direccion(double x, double y, double z) : CoordH(x,y,z,0) {};
    //Constructor vectorial
    Direccion(double c[3]) : CoordH(c[0],c[1],c[2],0) {};

    // OPERADORES
    Direccion operator+(Direccion d);
    Punto operator+(Punto p);
    Direccion operator-(Direccion d);
    double operator*(Direccion d); // Producto escalar
    Direccion operator*(double k);
    Direccion operator/(double k);
    Direccion operator-();

    bool operator==(Direccion d);
    bool operator!=(Direccion d);

    double operator[](int i);

    double modulo();
    double modulo2();

    // Devuelve la direccion normalizada
    Direccion normalizado();
    // Devuelve el producto vectorial de la direccion por d
    Direccion productoVectorial(Direccion d);
};

ostream& operator<<(ostream& os, const Direccion& d);
Direccion operator*(double k, Direccion d);

// ------------------------ MATRIZ TRANSFORMACION -------------------------
class MatrizT {
    public:
    double M[4][4];
    // Constructor por defecto
    MatrizT();
    // Constructor d*In
    MatrizT(double d);
    // Constructor de 4 componentes
    MatrizT(double m[4][4]);
    // Constructor de 3 direcciones y un punto
    MatrizT(Direccion d, Direccion u, Direccion v, Punto o);

    // OPERADORES
    MatrizT operator+(MatrizT m);
    MatrizT operator-(MatrizT m);
    MatrizT operator-();
    MatrizT operator*(double k);
    MatrizT operator*(MatrizT m);
    Punto operator*(Punto p);
    Direccion operator*(Direccion d);
};

    
// ------------------------------- OPERADORES -------------------------------
ostream& operator<<(ostream& os, const MatrizT& m);

MatrizT operator*(double k, MatrizT m);
Punto operator*(Punto p, MatrizT m);
Direccion operator*(Direccion d, MatrizT m);

// ---------------- OPERACIONES DE MATRICES DE TRANFORMACION ---------------
MatrizT identidad();

MatrizT traslacion(Direccion d);
MatrizT invTraslacion(Direccion d);

MatrizT escala(double sx, double sy, double sz);
MatrizT invEscala(double sx, double sy, double sz);

MatrizT rotacion(double th, int eje);

MatrizT inversa(MatrizT m);

#endif // COORDENADAS_H