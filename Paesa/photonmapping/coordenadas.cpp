// ------------------------------------------------
// Autores: Santiago Illa Criado y Carlos Paesa Lía
// Fecha: enero de 2024
// ------------------------------------------------
#include "coordenadas.h"
#include <iomanip>
#include <cmath>

// ------------------------ COORD HOMOGENEAS -------------------------
ostream& operator<<(ostream& os, const CoordH& c) {
    os << "{" << c.h[0] << ", " << c.h[1] << ", " << c.h[2] <<  ", " << c.h[3] <<"}";
    return os;
}

// ----------------------------- PUNTO -----------------------------
Punto Punto::operator+(Direccion rhs) {
    return Punto(h[0] + rhs.h[0], h[1] + rhs.h[1], h[2] + rhs.h[2]);
}

Punto Punto::operator-(Direccion rhs) {
    return Punto(h[0] - rhs.h[0], h[1] - rhs.h[1], h[2] - rhs.h[2]);
}

Direccion Punto::operator-(Punto rhs) {
    return Direccion(h[0] - rhs.h[0], h[1] - rhs.h[1], h[2] - rhs.h[2]);
}

Punto Punto::operator*(double rhs) {
    return Punto(h[0] * rhs, h[1] * rhs, h[2] * rhs);
}

Punto operator*(double lhs, Punto rhs) {
    return Punto(lhs * rhs.h[0], lhs * rhs.h[1], lhs * rhs.h[2]);
}

Punto Punto::operator/(double rhs) {
    return Punto(h[0] / rhs, h[1] / rhs, h[2] / rhs);
}

Punto Punto::operator-() {
    return Punto(-h[0], -h[1], -h[2]);
}

bool Punto::operator==(Punto rhs) {
    return (h[0] == rhs.h[0] && h[1] == rhs.h[1] && h[2] == rhs.h[2]);
}

bool Punto::operator!=(Punto rhs) {
    return (h[0] != rhs.h[0] || h[1] != rhs.h[1] || h[2] != rhs.h[2]);
}

const double& Punto::operator[](size_t i) const{
    return this->h[i];
}

ostream& operator<<(ostream& os, const Punto& p) {
    os << fixed << setprecision(2) << "(" << p.h[0] << ", " << p.h[1] << ", " << p.h[2] <<  ", " << p.h[3] <<")";
    return os;
}

// ----------------------------- DIRECCION -----------------------------
Direccion Direccion::operator+(Direccion rhs) {
    return Direccion(h[0] + rhs.h[0], h[1] + rhs.h[1], h[2] + rhs.h[2]);
}

Punto Direccion::operator+(Punto rhs) {
    return Punto(h[0] + rhs.h[0], h[1] + rhs.h[1], h[2] + rhs.h[2]);
}

Direccion Direccion::operator-(Direccion rhs) {
    return Direccion(h[0] - rhs.h[0], h[1] - rhs.h[1], h[2] - rhs.h[2]);
}

double Direccion::operator*(Direccion rhs) {
    return h[0] * rhs.h[0] + h[1] * rhs.h[1] + h[2] * rhs.h[2];
}

Direccion Direccion::operator*(double rhs) {
    return Direccion(h[0] * rhs, h[1] * rhs, h[2] * rhs);
}

Direccion operator*(double lhs, Direccion rhs) {
    return Direccion(lhs * rhs.h[0], lhs * rhs.h[1], lhs * rhs.h[2]);
}

Direccion Direccion::operator/(double rhs) {
    return Direccion(h[0] / rhs, h[1] / rhs, h[2] / rhs);
}

Direccion Direccion::operator-() {
    return Direccion(-h[0], -h[1], -h[2]);
}

bool Direccion::operator==(Direccion rhs) {
    return (h[0] == rhs.h[0] && h[1] == rhs.h[1] && h[2] == rhs.h[2]);
}

bool Direccion::operator!=(Direccion rhs) {
    return (h[0] != rhs.h[0] || h[1] != rhs.h[1] || h[2] != rhs.h[2]);
}

double Direccion::operator[](int i) {
    return h[i];
}

ostream& operator<<(ostream& os, const Direccion& d) {
    os << fixed << setprecision(2) << "<" << d.h[0] << ", " << d.h[1] << ", " << d.h[2] <<  ", " << d.h[3] <<">";
    return os;
}

double Direccion::modulo(){
    return sqrt(h[0]*h[0] + h[1]*h[1] + h[2]*h[2]);
}

double Direccion::modulo2(){
    return h[0]*h[0] + h[1]*h[1] + h[2]*h[2];
}

Direccion Direccion::normalizado() {
    double m = modulo();
    if (m == 0) {return Direccion();}
    else {
        Direccion d(h[0]/m, h[1]/m, h[2]/m);
        return d;
    }
}

Direccion Direccion::productoVectorial(Direccion d) {
    Direccion res = Direccion();
    res.h[0] = h[1]*d.h[2] - h[2]*d.h[1];
    res.h[1] = h[2]*d.h[0] - h[0]*d.h[2];
    res.h[2] = h[0]*d.h[1] - h[1]*d.h[0];
    return res;
}

// ------------------------ MATRIZ TRANSFORMACION -------------------------
MatrizT::MatrizT() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            M[i][j] = 0;
        }      
    }
}

MatrizT::MatrizT(double d) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            M[i][j] = d;
        }     
    }
}

MatrizT::MatrizT(double m[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            M[i][j] = m[i][j];
        }         
    }
}

MatrizT::MatrizT(Direccion d, Direccion u, Direccion v, Punto o) {
    M[0][0] = d.h[0]; M[0][1] = u.h[0]; M[0][2] = v.h[0]; M[0][3] = o.h[0];
    M[1][0] = d.h[1]; M[1][1] = u.h[1]; M[1][2] = v.h[1]; M[1][3] = o.h[1];
    M[2][0] = d.h[2]; M[2][1] = u.h[2]; M[2][2] = v.h[2]; M[2][3] = o.h[2];
    M[3][0] = 0;      M[3][1] = 0;      M[3][2] = 0;      M[3][3] = 1;
}

MatrizT MatrizT::operator+(MatrizT m) {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            res.M[i][j] = M[i][j] + m.M[i][j];
        }
    }
    return res;
}

MatrizT MatrizT::operator-(MatrizT m) {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            res.M[i][j] = M[i][j] - m.M[i][j];
        }
    }
    return res;
}

MatrizT MatrizT::operator-() {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            res.M[i][j] = -M[i][j];
        }
    }
    return res;
}

MatrizT MatrizT::operator*(double d) {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            res.M[i][j] = M[i][j] * d;
        }
    }
    return res;
}

MatrizT operator*(double d, MatrizT m) {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            res.M[i][j] = m.M[i][j] * d;
        }
    }
    return res;
}

MatrizT MatrizT::operator*(MatrizT rhs) {
    MatrizT res = MatrizT();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            for (int k = 0; k < 4; k++) {
                res.M[i][j] += M[i][k] * rhs.M[k][j];
            }
        }
    }
    return res;
}

Punto MatrizT::operator*(Punto rhs) {
    Punto res = Punto();
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            res.h[i] += M[i][k] * rhs.h[k];
        }
    }
    return res;
}

Punto operator*(Punto lhs, MatrizT rhs) {
    Punto res = Punto();
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            res.h[i] += lhs.h[k] * rhs.M[k][i];
        }
    }
    return res;
}

Direccion MatrizT::operator*(Direccion rhs) {
    Direccion res = Direccion();
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            res.h[i] += M[i][k] * rhs.h[k];
        }
    }
    return res;
}

Direccion operator*(Direccion lhs, MatrizT rhs) {
    Direccion res = Direccion();
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            res.h[i] += lhs.h[k] * rhs.M[k][i];
        }
    }
    return res;
}

ostream &operator<<(ostream &os, const MatrizT &m) {
    os << fixed << setprecision(2) << endl;
    for (int i = 0; i < 4; i++) {
        os << "| ";
        for (int j = 0; j < 4; j++) {
            os << m.M[i][j] << " ";
        }
        os << "|" << endl;
    }
    return os;
}

// ---------------- OPERACIONES DE MATRICES DE TRANFORMACION ---------------
MatrizT identidad() {
    MatrizT res = MatrizT();
    res.M[0][0] = 1;
    res.M[1][1] = 1;
    res.M[2][2] = 1;
    res.M[3][3] = 1;
    return res;
}

MatrizT traslacion(Direccion d) {
    MatrizT res = MatrizT();
    res.M[0][3] = d.h[0];
    res.M[1][3] = d.h[1];
    res.M[2][3] = d.h[2];
    return res;
}

MatrizT invTraslacion(Direccion d) {
    MatrizT res = MatrizT();
    res.M[0][3] = -d.h[0];
    res.M[1][3] = -d.h[1];
    res.M[2][3] = -d.h[2];
    return res;
}

MatrizT escala(double sx, double sy, double sz) {
    MatrizT res = MatrizT();
    res.M[0][0] = sx;
    res.M[1][1] = sy;
    res.M[2][2] = sz;
    return res;
}

MatrizT invEscala(double sx, double sy, double sz) {
    MatrizT res = MatrizT();
    res.M[0][0] = 1/sx;
    res.M[1][1] = 1/sy;
    res.M[2][2] = 1/sz;
    return res;
}

MatrizT rotacion(double th, int eje) {
    MatrizT res = MatrizT();
    switch (eje) {
    case 1: // eje x
        res.M[1][1] = cos(th);
        res.M[1][2] = -sin(th);
        res.M[2][1] = sin(th);
        res.M[2][2] = cos(th);
        break;
    case 2: // eje y
        res.M[0][0] = cos(th);
        res.M[0][2] = sin(th);
        res.M[2][0] = -sin(th);
        res.M[2][2] = cos(th);
        break;
    case 3: // eje z
        res.M[0][0] = cos(th);
        res.M[0][1] = -sin(th);
        res.M[1][0] = sin(th);
        res.M[1][1] = cos(th);
        break;
    default:
        break;
    }
    return res;
}

// --------------------------
// Tomado de geekforgeeks.org
// --------------------------
// Function to get cofactor of A[p][q] in temp[][]. n is
// current dimension of A[][]
void getCofactor(double A[4][4], double temp[4][4], int p, int q,
                 int n) {
    int i = 0, j = 0;
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            //  Copying into temporary matrix only those
            //  element which are not in given row and
            //  column
            if (row != p && col != q) {
                temp[i][j++] = A[row][col];
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}
 
/* Recursive function for finding determinant of matrix.
   n is current dimension of A[][]. */
int determinant(double A[4][4], int n) {
    int D = 0; // Initialize result
    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];
    double temp[4][4]; // To store cofactors
    int sign = 1; // To store sign multiplier
    // Iterate for each element of first row
    for (int f = 0; f < n; f++) {
        // Getting Cofactor of A[0][f]
        getCofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1);
        // terms are to be added with alternate sign
        sign = -sign;
    }
    return D;
}

// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(double A[4][4], double adj[4][4]){
    // temp is used to store cofactors of A[][]
    int sign = 1; 
    double temp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Get cofactor of A[i][j]
            getCofactor(A, temp, i, j, 4);
 
            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i + j) % 2 == 0) ? 1 : -1;
 
            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign) * (determinant(temp, 4 - 1));
        }
    }
}

// Function to calculate and store inverse, returns false if
// matrix is singular
bool inverse(double A[4][4], double inverse[4][4]) {
    // Find determinant of A[][]
    double det = determinant(A,4);
    if (det == 0) {
        return false;
    }
    // Find adjoint
    double adj[4][4];
    adjoint(A, adj);
    // Find Inverse using formula "inverse(A) =
    // adj(A)/det(A)"
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            inverse[i][j] = adj[i][j] / double(det);
 
    return true;
}

MatrizT inversa(MatrizT m) {
    MatrizT res = MatrizT();
    if (inverse(m.M, res.M))
        return res;
    else return MatrizT();
}
