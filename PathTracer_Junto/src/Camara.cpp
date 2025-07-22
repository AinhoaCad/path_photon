#include <vector>
#include "Operations.hpp"
#include "Camara.hpp"
#include "Material.hpp"
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

Image Camera::generateImage(const std::vector<Primitive*>& primitives) {
    Image image(pixelWidth, pixelHeight);
    for (int i = 0; i < pixelHeight; i++) {
        for (int j = 0; j < pixelWidth; j++) {
            Ray ray = generateRay(i, j, "uno");
            float t = -1;
            float mejorT = -1;
            Pixel p = {0, 0, 0}; // Si no intersecta, el color del píxel será este
            for (auto& primitive : primitives) {
                if (primitive->intersect(ray, t)) {
                    if ((t < mejorT || mejorT == -1) && t > 0) {
                        mejorT = t;
                        p = primitive->getMaterial().toPixel(); // Obtenemos el color del material de la primitiva
                    }
                }
            }
            image.setPixel(i, j, p);
        }
    }
    return image;
}

Image Camera::generateImageMultipleRays(const std::vector<Primitive*>& primitives, int numRays) {
    Image image(pixelWidth, pixelHeight);
    srand(static_cast<unsigned>(time(NULL)));

    int numPrimitives = static_cast<int>(primitives.size());
    std::vector<float> t(numRays, -1);
    std::vector<float> mejorT(numRays, -1);
    std::vector<int> mejorPrimitiva(numRays, -1);

    for (int i = 0; i < pixelHeight; i++) {
        for (int j = 0; j < pixelWidth; j++) {

            for (int r = 0; r < numRays; r++) {
                Ray ray = generateRay(i, j, "varios");
                t[r] = -1;
                mejorT[r] = -1;

                for (int k = 0; k < numPrimitives; k++) {
                    if (primitives[k]->intersect(ray, t[r])) {
                        if ((t[r] < mejorT[r] || mejorT[r] == -1) && t[r] > 0) {
                            mejorT[r] = t[r];
                            mejorPrimitiva[r] = k;
                        }
                    }
                }
            }

            Pixel p = {0, 0, 0}; // Color por defecto si no hay intersección

            for (int r = 0; r < numRays; r++) {
                if (mejorT[r] != -1) {
                    Pixel p2 = primitives[mejorPrimitiva[r]]->getMaterial().toPixel(); // Obtenemos el color del material de la primitiva
                    p.r += p2.r;
                    p.g += p2.g;
                    p.b += p2.b;
                }
            }

            // Promediamos el color
            p.r /= numRays;
            p.g /= numRays;
            p.b /= numRays;

            image.setPixel(i, j, p);
        }
    }

    return image;
}
