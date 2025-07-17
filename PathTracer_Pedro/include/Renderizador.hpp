#ifndef RENDERIZADOR_HPP
#define RENDERIZADOR_HPP

#include "Scene.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
#include "Vector3.hpp"
#include "Color.hpp"


class Renderizador {
public:
    Renderizador(const Scene& escena);

    // Método principal
    Color calcularColorRayo(const Ray& rayo, int depth) const;

    //getScene
    const Scene& getScene() const { return escena_; }

private:
    const Scene& escena_; // Referencia a la escena

    Vector3 random_in_hemisphere(const Vector3& normal) const;
    bool encontrarInterseccion(const Ray& rayo, float& tMin, Intersection& interseccion) const;
};

#endif // RENDERIZADOR_HPP

