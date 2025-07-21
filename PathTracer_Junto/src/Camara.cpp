#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include <ctime> // Para usar srand
#include <cstdlib> // Para generar números aleatorios


Camera::Camera(Point origen, Direction l, Direction u, Direction f, int anchp, int altp)
    : origen(origen), left(l), up(u), forward(f), pixelWidth(anchp), pixelHeight(altp) {
    // Calcular dimensiones físicas del plano de la imagen
    onepixelWidth = anchoPlano / pixelWidth;
    onepixelHeight = altoPlano / pixelHeight;
}

// Método para generar un rayo hacia el centro de un píxel dado (i, j)
Ray Camera::generateRay(int i, int j, std::string n) {
    float pixelCenterX, pixelCenterY;

    if(n == "uno"){
        // Calculamos las coordenadas del centro del píxel (i, j)
        pixelCenterX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2;
        pixelCenterY = - (j + 0.5f) * onepixelHeight + altoPlano / 2;
    }
    else if (n == "varios"){
        double aleatorioX = (double)rand() / RAND_MAX;   //0.0 y 1.0
        aleatorioX = aleatorioX * 2 - 1; // -1.0 y 1.0
        aleatorioX = onepixelWidth * aleatorioX;
            
        double aleatorioY = (double)rand() / RAND_MAX;   //0.0 y 1.0
        aleatorioY = aleatorioY * 2 - 1; // -1.0 y 1.0
        aleatorioY = onepixelWidth * aleatorioY;

        // Calculamos las coordenadas del centro del píxel (i, j)
        pixelCenterX = -(i + 0.5f) * onepixelWidth + anchoPlano / 2 + aleatorioX;
        pixelCenterY = - (j + 0.5f) * onepixelHeight + altoPlano / 2 + aleatorioY;
    }
    // El punto en el plano de la imagen en coordenadas de la cámara
    Direction rayDirection = Direction(pixelCenterX, pixelCenterY, 1);
    Operations::norm(rayDirection); // Aseguramos que el vector esté normalizado
    
    std::vector<Matrix> matrices = {left.getHomMatrix(), up.getHomMatrix(), forward.getHomMatrix(), origen.getHomMatrix()};
    Matrix m(matrices); // Creamos la matriz de cambio de base al sistema global

    // Multiplicamos la dirección del rayo por la matriz de cambio de base
    Matrix a = m * rayDirection.getHomMatrix();

    rayDirection = Direction(a); // Convertimos el resultado en un vector dirección
    return Ray(origen.toVector(), rayDirection.toVector()); // Devolvemos el rayo con origen en la cámara y dirección calculada
}

Image Camera::generateImage(Primitive* primitives[], int numPrimitives) {
    Image image(pixelWidth, pixelHeight);
    for (int i = 0; i < pixelHeight; ++i) {
        for (int j = 0; j < pixelWidth; ++j) {
            Ray ray = generateRay(i, j, "uno");
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

Image Camera::generateImageMultipleRays(Primitive* primitives[], int numPrimitives, int numRays) {
        Image image(pixelWidth, pixelHeight);
        srand(time(NULL));
        float t[numRays];
        float mejorT[numRays];
        int mejorPrimitiva[numRays];
        for (int i = 0; i < pixelHeight; i++) {
            for (int j = 0; j < pixelWidth; j++) {
                for (int r = 0; r < numRays; r++) {
                    Ray ray = generateRay(i, j, "varios");
                    //cout << "Rayo " << r << " generado" << endl;
                    t[r] = -1;
                    mejorT[r] = -1;
                    // Mirar si el rayo intersecta con algún objeto
                    // Si intersecta, calcular el color del píxel
                    for (int k = 0; k < numPrimitives; k++) {
                        if (primitives[k]->intersect(ray, t[r])) {
                            // Si el objeto intersecta, el color del píxel
                            // es el color del objeto si es el más cercano
                            if ((t[r] < mejorT[r] || mejorT[r] == -1) && t[r] > 0) {
                                mejorT[r] = t[r];
                                mejorPrimitiva[r] = k;
                                //cout << "Intersecta con objeto " << k << " en t = " << t << endl;
                            }
                        }
                    }
                }
                //cout << "Todos los rayos de un pixel generados" << endl;
                Pixel p = {0, 0, 0}; // Si no intersecta, el color del píxel será este
                for (int r = 0; r < numRays; r++) {
                    if (mejorT[r] != -1) { //Si ha intersectado con el objeto
                        Pixel p2 = primitives[mejorPrimitiva[r]]->getColor();
                        p.r += p2.r;
                        p.g += p2.g;
                        p.b += p2.b;
                    }
                }
                //Nos quedamos con la media de los colores
                p.r /= numRays;
                p.g /= numRays;
                p.b /= numRays;
                image.setPixel(i,j, p);
            }
        }
        return image;
    }