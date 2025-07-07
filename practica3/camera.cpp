#include <vector>
#include "../practica1/operations.h"
#include "camera.h"

Camera::Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp)
    : origen(origen), left(l), up(u), forward(f), pixelWidth(anchp), pixelHeight(altp) {
    // Calcular dimensiones físicas del plano de la imagen
    onepixelWidth = anchoPlano / pixelWidth;
    onepixelHeight = altoPlano / pixelHeight;
}

// Método para generar un rayo hacia el centro de un píxel dado (i, j)
Ray Camera::generateRay(int i, int j) {
    // Calculamos las coordenadas del centro del píxel (i, j)
    float pixelCenterX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2;
    float pixelCenterY = - (j + 0.5f) * onepixelHeight + altoPlano / 2;


    // El punto en el plano de la imagen en coordenadas de la cámara
    Direction rayDirection = Direction(pixelCenterX, pixelCenterY, 1);
    Operations::norm(rayDirection); // Aseguramos que el vector esté normalizado
    
    std::vector<Matrix> matrices = {left.getHomMatrix(), up.getHomMatrix(), forward.getHomMatrix(), origen.getHomMatrix()};
    Matrix m(matrices); // Creamos la matriz de cambio de base al sistema global

    // Multiplicamos la dirección del rayo por la matriz de cambio de base
    Matrix a = m * rayDirection.getHomMatrix();

    rayDirection = Direction(a); // Convertimos el resultado en un vector dirección
    return Ray(origen, rayDirection); // Devolvemos el rayo con origen en la cámara y dirección calculada
}

Image Camera::generateImage(Primitive* primitives[], int numPrimitives) {
    Image image(pixelWidth, pixelHeight);
    for (int i = 0; i < pixelHeight; ++i) {
        for (int j = 0; j < pixelWidth; ++j) {
            Ray ray = generateRay(i, j);
            float t = -1;
            float mejorT = -1;
            Pixel p = {0, 0, 0}; // Si no intersecta, el color del píxel será este
            // Mirar si el rayo intersecta con algún objeto
            // Si intersecta, calcular el color del píxel
            for (int k = 0; k < numPrimitives; k++) {
                if (primitives[k]->intersect(ray, t)) {
                    // Si el objeto intersecta, el color del píxel
                    // es el color del objeto si es el más cercano
                    if ((t < mejorT || mejorT == -1) && t > 0) {
                        mejorT = t;
                        p = primitives[k]->getColor();
                        //cout << "Intersecta con objeto " << k << " en t = " << t << endl;
                    }
                }
            }
            image.setPixel(i,j, p);
        }
    }
    return image;
}