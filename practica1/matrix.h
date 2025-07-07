#include <iostream>
#include <vector>

class Matrix {
private:
    std::vector<std::vector<float>> data;
    int rows, cols;
public:
    // Constructor: matriz vacía con filas y columnas dadas
    Matrix(int rows, int cols);

    // Constructor: matriz con datos dados en vector 2D
    Matrix(const std::vector<std::vector<float>>& values);

    // Constructor: concatena matrices horizontalmente
    Matrix(const std::vector<Matrix>& matrices);

    int getRows() const;
    int getCols() const;
    float getValue(int i, int j) const;

    float& operator()(int i, int j);
    const float& operator()(int i, int j) const;

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& m) const;

    Matrix transpose() const;

    static Matrix identity(int size);
    
    // Sobrecarga operador de salida para imprimir
    friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
};
