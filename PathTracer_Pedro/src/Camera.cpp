#include "Camera.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>

// Genera un número aleatorio entre 0 y 1
double randomd() {
    return rand() / (RAND_MAX + 1.0);
}

// Genera un número aleatorio entre un mínimo y un máximo
double random_double(double min, double max) {
    return min + (max - min) * randomd();
}

// Constructor por defecto
Camera::Camera() {
    origen = Vector3(0, 0, -3.5);       // Origen de la cámara
    l = Vector3(-1, 0, 0);             // Izquierda
    u = Vector3(0, 1, 0);              // Arriba
    f = Vector3(0, 0, 3);              // Frente
    numPixelsAlto = 256;               // Resolución vertical
    numPixelsAncho = 256;              // Resolución horizontal
    nMuestras = 10;                    // Muestras por píxel
    alturaPixel = 0;                   // Calculado en Initialize
    anchuraPixel = 0;                  // Calculado en Initialize
}

// Inicializa las dimensiones de los píxeles
void Camera::Initialize() {
    alturaPixel = (2 * u.length()) / numPixelsAlto;
    anchuraPixel = (2 * l.length()) / numPixelsAncho;
    std::cout << "Dimensiones de píxeles: altura = " << alturaPixel << ", anchura = " << anchuraPixel << std::endl;
    std::cout << "Dimensiones de la cámara: " << numPixelsAncho << "x" << numPixelsAlto << std::endl;
}

// Crea un rayo para un píxel específico
Ray Camera::crearRayo(double i, double j) {
    Vector3 dir = f + u - u * (alturaPixel * j) + l - l * (anchuraPixel * i);
    dir = dir.normalized();
    std::cout << "Rayo generado: origen = " << origen << ", dirección = " << dir << std::endl;
    return Ray(origen, dir);
}

// Crea un rayo con un desplazamiento aleatorio para antialiasing
Ray Camera::crearRayoMuestra(double i, double j) {
    double xDev = -0.5 + randomd() + i;
    double yDev = -0.5 + randomd() + j;

    Vector3 dir = f + u - u * (alturaPixel * yDev) + l - l * (anchuraPixel * xDev);
    dir = dir.normalized();
    return Ray(origen, dir);
}

void Camera::guardarImagen(const std::string& archivoSalida, const std::vector<Color>& buffer, int ancho, int alto) const {
    std::ofstream output(archivoSalida);

    if (!output) {
        std::cerr << "Error al abrir el archivo de salida: " << archivoSalida << std::endl;
        return;
    }

    // Escribir el encabezado PPM
    output << "P3\n";  // P3 indica que es un archivo en formato texto
    output << ancho << " " << alto << "\n";
    output << "255\n";  // Valor máximo para cada componente de color (RGB)

    // Escribir los datos de los píxeles
    for (int i = 0; i < ancho * alto; ++i) {
        const Color& pixel = buffer[i];
        // Asegúrate de que los valores de color estén dentro del rango [0, 255]
        output << static_cast<int>(std::min(255.0f, std::max(0.0f, pixel.R * 255.0f))) << " ";
        output << static_cast<int>(std::min(255.0f, std::max(0.0f, pixel.G * 255.0f))) << " ";
        output << static_cast<int>(std::min(255.0f, std::max(0.0f, pixel.B * 255.0f))) << "\n";
    }
}



// Renderiza una imagen de prueba (sin escena aún)
// Renderizar imagen
void Camera::render(const std::string& archivoSalida, Renderizador& renderizador, int maxDepth) {
    std::vector<Color> buffer(numPixelsAncho * numPixelsAlto);
    std::cout << "Renderizando imagen con profundidad máxima: " << maxDepth << std::endl;

    for (int y = 0; y < numPixelsAlto; ++y) {
        for (int x = 0; x < numPixelsAncho; ++x) {
            Color pixelColor(0, 0, 0);
            for (int s = 0; s < nMuestras; ++s) {
                Ray ray = crearRayoMuestra(x, y);
                pixelColor += renderizador.calcularColorRayo(ray, maxDepth);
            }
            buffer[y * numPixelsAncho + x] = pixelColor / nMuestras;
        }
    }

    std::string output(archivoSalida);
    guardarImagen(output, buffer, numPixelsAncho, numPixelsAlto);
}