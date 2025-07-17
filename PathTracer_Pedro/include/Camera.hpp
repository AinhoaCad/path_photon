#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vector3.hpp"
#include "Ray.hpp"
#include "Color.hpp"
#include "Renderizador.hpp"
#include <fstream>
#include <vector>

class Camera {
public:
    Vector3 origen;      // Origen de la cámara
    Vector3 l;           // Vector hacia la izquierda
    Vector3 u;           // Vector hacia arriba
    Vector3 f;           // Vector hacia el frente

    int numPixelsAncho;  // Número de píxeles horizontal
    int numPixelsAlto;   // Número de píxeles vertical
    int nMuestras;       // Muestras por píxel

    double alturaPixel;  // Altura de cada píxel
    double anchuraPixel; // Anchura de cada píxel

    // Constructor
    Camera();

    // Métodos
    void Initialize();
    Ray crearRayo(double i, double j);
    Ray crearRayoMuestra(double i, double j);
    // Declaración de la función guardarImagen
    void guardarImagen(const std::string& archivoSalida, const std::vector<Color>& buffer, int ancho, int alto) const;
    void render(const std::string& archivoSalida, Renderizador& renderizador, int maxDepth);
};

#endif // CAMERA_HPP


