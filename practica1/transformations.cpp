#include <iostream>
#include <cmath>
#include "transformations.h"

// matriz de traslación 4x4
Matrix transformations::translation(float tx, float ty, float tz) {
    Matrix translationMatrix = Matrix::identity(4);
    translationMatrix(0, 3) = tx; // Asigna t_x
    translationMatrix(1, 3) = ty; // Asigna t_y
    translationMatrix(2, 3) = tz; // Asigna t_z
    return translationMatrix;
}

// rotacion 4x4 en el ejeX
Matrix transformations::rotationX(float angle) {
    Matrix rotationMatrix = Matrix::identity(4);
    rotationMatrix(1, 1) = cos(angle);
    rotationMatrix(1, 2) = -sin(angle);
    rotationMatrix(2, 1) = sin(angle);
    rotationMatrix(2, 2) = cos(angle);
    return rotationMatrix;
}

// rotacion 4x4 en el ejeY
Matrix transformations::rotationY(float angle) {
    Matrix rotationMatrix = Matrix::identity(4);
    rotationMatrix(0, 0) = cos(angle);
    rotationMatrix(0, 2) = -sin(angle);
    rotationMatrix(2, 0) = sin(angle);
    rotationMatrix(2, 2) = cos(angle);
    return rotationMatrix;
}

// rotacion 4x4 en el ejeZ
Matrix transformations::rotationZ(float angle) {
    Matrix rotationMatrix = Matrix::identity(4);
    rotationMatrix(0, 0) = cos(angle);
    rotationMatrix(0, 1) = -sin(angle);
    rotationMatrix(1, 0) = sin(angle);
    rotationMatrix(1, 1) = cos(angle);
    return rotationMatrix;
}
    
// matriz escalada 4x4
Matrix transformations::scale(float sx, float sy, float sz) {
    Matrix scaleMatrix = Matrix::identity(4);
    scaleMatrix(0, 0) = sx; // Asigna sx
    scaleMatrix(1, 1) = sy; // Asigna sy
    scaleMatrix(2, 2) = sz; // Asigna sz
    return scaleMatrix;
}

// Cambio de base de una matriz
Matrix transformations::changeBase(Direction u, Direction v,Direction w, Point origin) {
    Matrix changeBaseMatrix = Matrix::identity(4);
    // Asignar las coordenadas de la dirección u (x, y, z) a la primera fila
    changeBaseMatrix(0, 0) = u.getX(); // sx
    changeBaseMatrix(0, 1) = u.getY(); // sy
    changeBaseMatrix(0, 2) = u.getZ(); // sz

    // Asignar las coordenadas de la dirección v (x, y, z) a la segunda fila
    changeBaseMatrix(1, 0) = v.getX();
    changeBaseMatrix(1, 1) = v.getY();
    changeBaseMatrix(1, 2) = v.getZ();

    // Asignar las coordenadas de la dirección w (x, y, z) a la tercera fila
    changeBaseMatrix(2, 0) = w.getX();
    changeBaseMatrix(2, 1) = w.getY();
    changeBaseMatrix(2, 2) = w.getZ();

    // Asignar las coordenadas del punto origen a la cuarta columna
    changeBaseMatrix(0, 3) = origin.getX();
    changeBaseMatrix(1, 3) = origin.getY();
    changeBaseMatrix(2, 3) = origin.getZ();
            
    return changeBaseMatrix;
}

// matriz transformada inversa
Matrix transformations::inverseTranslation(float tx, float ty, float tz) {
    return translation(-tx, -ty, -tz);
}

// matriz escalada inversa
Matrix transformations::inverseScale(float sx, float sy, float sz) {
    return scale(1.0f / sx, 1.0f / sy, 1.0f / sz);
}