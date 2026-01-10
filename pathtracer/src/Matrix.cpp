#include "matrix.hpp"
#include <iostream>

// Constructor: Inicializa una matriz con filas y columnas dadas (VACÍA)
Matrix::Matrix(int rows, int cols) : data(rows, std::vector<float>(cols, 0)), rows(rows), cols(cols) {}

// Constructor: Inicializa una matriz con un vector 2D dado (CON DATOS QUE PASAS)
Matrix::Matrix(const std::vector<std::vector<float>>& values) : data(values) {
    rows = values.size();
    cols = values[0].size();
}

// Constructor: Inicializa una matriz concatenando otras matrices horizontalmente
Matrix::Matrix(const std::vector<Matrix>& matrices) {
    if (matrices.empty()) {
        throw std::invalid_argument("Debe proporcionar al menos una matriz");
    }

    rows = matrices[0].getRows();
    cols = 0;

    for (const auto& mat : matrices) {
        if (mat.getRows() != rows) {
            throw std::invalid_argument("Todas las matrices deben tener el mismo número de filas");
        }
        cols += mat.getCols();
    }

    data.resize(rows, std::vector<float>(cols, 0));

    int currentCol = 0;
    for (const auto& mat : matrices) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < mat.getCols(); c++) {
                data[r][currentCol + c] = mat.getValue(r, c);
            }
        }
        currentCol += mat.getCols();
    }
}

int Matrix::getRows() const {
    return rows;
}

int Matrix::getCols() const {
    return cols;
}

float Matrix::getValue(int i, int j) const {
    return data[i][j];
}

float& Matrix::operator()(int i, int j) {
    if (i < 0 || i >= rows || j < 0 || j >= cols) {
        throw std::out_of_range("Índice fuera de rango");
    }
    return data[i][j];
}

const float& Matrix::operator()(int i, int j) const {
    if (i < 0 || i >= rows || j < 0 || j >= cols) {
        throw std::out_of_range("Índice fuera de rango");
    }
    return data[i][j];
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Las dimensiones de las matrices no coinciden");
    }
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Las dimensiones de las matrices no coinciden");
    }
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& m) const {
    if (cols != m.rows) {
        throw std::invalid_argument("Las matrices no son multiplicables");
    }
    Matrix result(rows, m.cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            for (int k = 0; k < cols; k++) {
                result.data[i][j] += data[i][k] * m.data[k][j];
            }
        }
    }
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[j][i] = data[i][j];
        }
    }
    return result;
}

Matrix Matrix::identity(int size) {
    Matrix result(size, size);
    for (int i = 0; i < size; i++) {
        result(i, i) = 1;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; i++) {
        for (int j = 0; j < matrix.cols; j++) {
            os << matrix.data[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}
