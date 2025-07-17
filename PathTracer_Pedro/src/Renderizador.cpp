#include "Renderizador.hpp"
#include <limits>
#include <cmath>
#include <cstdlib>

// Constructor
Renderizador::Renderizador(const Scene& escena)
    : escena_(escena) {}

// Función para generar un número aleatorio en el rango [min, max]
double randomdoub(double min, double max) {
    return min + (max - min) * (rand() / (RAND_MAX + 1.0));
}

// Genera un vector aleatorio dentro de un hemisferio alrededor de la normal
Vector3 Renderizador::random_in_hemisphere(const Vector3& normal) const {
    Vector3 randomVec;
    do {
        randomVec = Vector3(randomdoub(-1.0f, 1.0f), randomdoub(-1.0f, 1.0f), randomdoub(-1.0f, 1.0f));
    } while (randomVec.dotProduct(normal) <= 0);  // Aseguramos que el vector esté en el hemisferio correcto
    return randomVec.normalized();
}

// Encontrar intersección más cercana
bool Renderizador::encontrarInterseccion(const Ray& rayo, float& tMin, Intersection& interseccion) const {
    tMin = std::numeric_limits<float>::max();
    bool hit = false;

    // Iterar sobre todas las esferas en la escena
    for (const auto& esfera : escena_.spheres) {
        float t;
        if (esfera.intersect(rayo, t) && t < tMin) {
            tMin = t;
            interseccion = Intersection(esfera.getIntersectionPoint(rayo, t),
                                         esfera.getNormal(rayo, t),
                                         esfera.getMaterial(),
                                         t);
            hit = true;
        }
    }

    // Iterar sobre todos los planos en la escena
    for (const auto& plano : escena_.planes) {
        float t;
        if (plano.intersect(rayo, t) && t < tMin) {
            tMin = t;
            interseccion = Intersection(plano.getIntersectionPoint(rayo, t),
                                         plano.getNormal(),
                                         plano.getMaterial(),
                                         t);
            hit = true;
        }

    }

    //std::cout << "Intersección encontrada: " << hit << std::endl;

    return hit;
}

// Calcular color del rayo
Color Renderizador::calcularColorRayo(const Ray& rayo, int depth) const {
    // std::cout << "Calculando color del rayo con profundidad: " << depth << std::endl;
    if (depth <= 0) return Color(0, 0, 0);  // Si ya hemos alcanzado la profundidad máxima, devolvemos color negro

    float tMin;
    Intersection interseccion;
    if (!encontrarInterseccion(rayo, tMin, interseccion)) {
        return Color(0, 0, 0);  // Si no hay intersección, devolvemos el fondo (negro)
    }

    // Emisión directa: componente emitida por el material
    Color colorFinal = interseccion.material.Le;

    // Iluminación directa desde luces puntuales
    for (const auto& luz : escena_.lights) {
        Vector3 luzDir = (luz.getPosition() - interseccion.punto).normalized();
        float luzDistancia = (luz.getPosition() - interseccion.punto).length();

        // Verificar si hay sombras
        Ray sombraRay(interseccion.punto, luzDir);
        float tSombra;
        Intersection interseccionSombra;
        if (!encontrarInterseccion(sombraRay, tSombra, interseccionSombra) || tSombra > luzDistancia) {
            // Si no hay sombras, agregar iluminación difusa
            float nDotL = std::max(0.0f, interseccion.normal.dotProduct(luzDir));
            colorFinal += luz.getIntensity() * nDotL * interseccion.material.kd;
        }
    }

    // Reflexión especular
    if (interseccion.material.ks.R > 0 || interseccion.material.ks.G > 0 || interseccion.material.ks.B > 0) {
        Vector3 mul1 = interseccion.normal * 2;
        Vector3 reflectDir = rayo.direction() - mul1 * rayo.direction().dotProduct(interseccion.normal);
        Ray reflectRay(interseccion.punto, reflectDir);
        colorFinal += calcularColorRayo(reflectRay, depth - 1) * interseccion.material.ks;
    }

    // Refracción
    if (interseccion.material.kt.R > 0 || interseccion.material.kt.G > 0 || interseccion.material.kt.B > 0) {
        float eta = 1.0f / interseccion.material.ior;  // Índice de refracción
        Vector3 refractDir = rayo.direction().refract(interseccion.normal, eta);
        if (refractDir.length() > 0) {
            Ray refractRay(interseccion.punto, refractDir);
            colorFinal += calcularColorRayo(refractRay, depth - 1) * interseccion.material.kt;
        }
    }


    return colorFinal;
}




