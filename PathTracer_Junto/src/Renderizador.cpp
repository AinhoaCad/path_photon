#include <cstdlib> // Para rand()
#include "Renderizador.hpp"

// Función auxiliar: devuelve un desplazamiento aleatorio dentro de un pixel
float randomOffset(float pixelSize) {
    // rand() / RAND_MAX genera un número en [0,1]
    // Multiplicamos por 2 y restamos 1 → [-1,1]
    // Luego multiplicamos por el tamaño del píxel para obtener el offset real
    double r = (double)rand() / RAND_MAX;  
    return pixelSize * (r * 2.0 - 1.0);    
}

// Devuelve el color del material de la primitiva más cercana intersectada por el rayo.
// Si no hay intersección, devuelve negro.
Pixel calcularColorMasCercano(Ray& ray, const std::vector<Primitive*>& primitives) {
    float mejorT = -1.0f;
    //por defecto si no pilla primitiva es negro
    Pixel color = {0, 0, 0};

    for (auto& primitive : primitives) {
        float t = -1.0f;
        if (primitive->intersect(ray, t)) {
            if ((t < mejorT || mejorT < 0) && t > 0) {
                mejorT = t;
                color = primitive->getMaterial().toPixel();
            }
        }
    }
    return color;
}


Pixel promedioColores(const std::vector<Pixel>& colores) {
    if (colores.empty()) return {0,0,0};

    Pixel total = {0,0,0};
    for (const auto& c : colores) {
        total.r += c.r;
        total.g += c.g;
        total.b += c.b;
    }

    float n = static_cast<float>(colores.size());
    total.r /= n;
    total.g /= n;
    total.b /= n;
    return total;
}

// Devuelve la intersección más cercana del rayo con las primitivas
Interseccion calcularInterseccionMasCercana(Ray& ray, const std::vector<Primitive*>& primitives) {
    Interseccion inter;
    inter.hayInterseccion = false;
    inter.t = -1;
    inter.primitiva = nullptr;

    for (auto& prim : primitives) {
        float t = -1;
        if (prim->intersect(ray, t) && t > 0.001f && (t < inter.t || inter.t < 0)) {
            inter.hayInterseccion = true;
            inter.t = t;
            inter.primitiva = prim;
        }
    }

    if (inter.hayInterseccion && inter.primitiva) {
        inter.punto = Operations::add(ray.origin_.toPoint(), Operations::mult(ray.direction_.toDirection(), inter.t));
        inter.normal = inter.primitiva->getPointNormal(inter.punto);
    }

    return inter;
}

// Comprueba si un punto está en sombra respecto a una luz
bool estaEnSombra(const Point& punto, const Direction& normal, const Light& luz, const std::vector<Primitive*>& primitives) {
    Point shadowOrigin = Operations::add(punto, Operations::mult(normal, 0.001f)); // Evita autocontacto
    Point lightSample;
    Direction shadowDir;
    float maxDist;

    // Luz de área
    if (luz.width > 0 && luz.height > 0) {
        lightSample = luz.samplePointOnArea((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    } else {
        lightSample = luz.origen;
    }

    shadowDir = Operations::sub(lightSample, shadowOrigin);
    maxDist = Operations::mod(shadowDir);
    shadowDir = Operations::norm(shadowDir);

    Ray shadowRay(shadowOrigin.toVector(), shadowDir.toVector());

    for (auto& prim : primitives) {
        float t;
        if (prim->intersect(shadowRay, t) && t > 0.001f && t < maxDist) {
            return true; // Algo bloquea la luz
        }
    }
    return false;
}

// Calcula la iluminación de un punto en base a las luces visibles
Pixel calcularIluminacion(const Interseccion& inter, const std::vector<Light>& luces, const std::vector<Primitive*>& primitives) {
    Pixel colorFinal = inter.primitiva->getMaterial().toPixel();

    // Si el objeto emite luz, sumarla directamente
    if (inter.primitiva->emitsLight()) {
        Plane* emisiva = static_cast<Plane*>(inter.primitiva);
        Light areaLight = emisiva->getAreaLight();
        Pixel emision = areaLight.calcularRadianciaRectangular();
        colorFinal.r += emision.r;
        colorFinal.g += emision.g;
        colorFinal.b += emision.b;
    }

    for (const auto& luz : luces) {
        if (!estaEnSombra(inter.punto, inter.normal, luz, primitives)) {
            Pixel contrib;
            if (luz.width > 0 && luz.height > 0) {
                // Luz de área: promediamos varias muestras
                int muestras = 8;
                Pixel acumulado = {0, 0, 0};

                for (int i = 0; i < muestras; ++i) {
                    Point sample = luz.samplePointOnArea((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
                    Pixel ilum = luz.calcularLuzAreaSample(inter.punto, inter.normal, colorFinal, sample);
                    acumulado.r += ilum.r;
                    acumulado.g += ilum.g;
                    acumulado.b += ilum.b;
                }

                contrib.r = acumulado.r / muestras;
                contrib.g = acumulado.g / muestras;
                contrib.b = acumulado.b / muestras;
            } else {
                // Luz puntual
                contrib = luz.calcularLuz(inter.punto, inter.normal, colorFinal);
            }

            colorFinal.r = colorFinal.r + contrib.r;
            colorFinal.g = colorFinal.g + contrib.g;
            colorFinal.b = colorFinal.b + contrib.b;
        }
    }

    return colorFinal;
}