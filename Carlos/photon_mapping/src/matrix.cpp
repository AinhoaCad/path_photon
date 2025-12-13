#include "matrix.hpp"
#include <iomanip>

Matrix::Matrix() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            c[i][j]=0;
        }
    }
}

Matrix::Matrix(float elements[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            c[i][j]=elements[i][j];
        }
    }
}

Matrix::Matrix(const float elements[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            c[i][j]=elements[i][j];
        }
    }
}


ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    os << std::fixed << std::setprecision(1);  // Set precision for float display
    // Top boundary
    os << " / ";
    for (int j = 0; j < 4; ++j) {
        os << std::setw(4) << matrix.c[0][j] << " ";  // First row
    }
    os << "\\\n";  // Closing the top boundary
    
    // Middle rows
    for (int i = 1; i < 3; ++i) {
        os << " | ";  // Left boundary
        for (int j = 0; j < 4; ++j) {
            os << std::setw(4) << matrix.c[i][j] << " ";
        }
        os << "|\n";  // Right boundary
    }
    
    // Bottom boundary
    os << " \\ ";
    for (int j = 0; j < 4; ++j) {
        os << std::setw(4) << matrix.c[3][j] << " ";  // Last row
    }
    os << "/\n";  // Closing the bottom boundary
    
    return os;
}


Matrix operator*(const Matrix& M, float r) {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.c[i][j] = M.c[i][j] * r;
        }
    }
    return result;
}


Matrix operator*(const Matrix& M1, const Matrix& M2) {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.c[i][j] = M1.c[i][k] * M2.c[k][j];
            }
        }
    }
    return result;
}

array<float, 4> Matrix::operator*(const array<float, 4>& vec) {
    array<float, 4> result = {0, 0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i] += c[i][j] * vec[j];
        }
    }
    return result;
}


Matrix operator-(const Matrix& M1, const Matrix& M2) {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.c[i][j] = M1.c[i][j] - M2.c[i][j];
        }
    }
    return result;
}


Matrix operator+(const Matrix& M1, const Matrix& M2) {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.c[i][j] = M1.c[i][j] + M2.c[i][j];
        }
    }
    return result;
}


// Función auxiliar para calcular el determinante de una matriz 3x3
float determinant3x3(float matrix[3][3]) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
         - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
         + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

// Función para calcular el determinante de una matriz 4x4
float Matrix::determinant() const {
    float det = 0.0f;
    for (int i = 0; i < 4; ++i) {
        float minor[3][3];
        for (int j = 1; j < 4; ++j) {
            for (int k = 0, col = 0; k < 4; ++k) {
                if (k != i) {
                    minor[j - 1][col] = c[j][k];
                    col++;
                }
            }
        }
        float cofactor = ((i % 2 == 0) ? 1 : -1) * c[0][i] * determinant3x3(minor);
        det += cofactor;
    }
    return det;
}

Matrix cofactorMatrix(const Matrix& M) {
    Matrix cof;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float minor[3][3];
            for (int row = 0, m_row = 0; row < 4; ++row) {
                if (row != i) {
                    for (int col = 0, m_col = 0; col < 4; ++col) {
                        if (col != j) {
                            minor[m_row][m_col] = M.c[row][col];
                            m_col++;
                        }
                    }
                    m_row++;
                }
            }
            if (determinant3x3(minor)==0){
                cof.c[i][j]=determinant3x3(minor);
            }else{
                cof.c[i][j] = ((i + j) % 2 == 0 ? 1 : -1) * determinant3x3(minor);
            }
        }
    }
    return cof;
}

Matrix transpose(const Matrix& M) {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.c[i][j] = M.c[j][i];
        }
    }
    return result;
}


Matrix Matrix::inverse() const {
    float det = determinant();
    if (det == 0) {
        throw std::runtime_error("Error: No se puede calcular la inversa de una matriz con determinante 0");
    }

    Matrix cofactors = cofactorMatrix(*this);  // Matriz de cofactores
    Matrix adjugate = transpose(cofactors);    // Matriz adjunta es la transpuesta de la matriz de cofactores
    return adjugate * (1.0f / det);            // Inversa = adjugate / determinante
}
