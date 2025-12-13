#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <array>

using namespace std;

class Matrix{
    float c[4][4];
public:
    //Constructor por defecto
    Matrix();
    Matrix(float elements[4][4]);
    Matrix(const float elements[4][4]);
    friend ostream& operator<<(ostream& os, const Matrix& D);

    // Arithmetic operators
    friend Matrix operator*(const Matrix& M, float r);
    friend Matrix operator*(const Matrix& M1, const Matrix& M2);
    friend Matrix operator-(const Matrix& M1,const Matrix& M2);
    friend Matrix operator+(const Matrix& M1, const Matrix& M2);

    array<float, 4> operator*(const array<float, 4> &vec);

    float determinant() const;
    Matrix inverse() const;
    friend Matrix transpose(const Matrix& M);
    friend Matrix cofactorMatrix(const Matrix& M);
};

#endif