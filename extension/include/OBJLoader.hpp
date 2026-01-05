#pragma once

#include <string>
#include <vector>
#include "Triangle.hpp"
#include "Scene.hpp"
#include "Material.hpp"

class OBJLoader {
public:
    // Carga un archivo OBJ y añade los triángulos a la escena
    // Devuelve true si tuvo éxito, false si hubo error
    static bool loadOBJ(
        const std::string& filepath,
        Scene& scene,
        const Material& material,
        const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f),
        const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f),
        bool smoothNormals = true
    );

    // Versión alternativa que devuelve directamente los triángulos
    static std::vector<Triangle> loadOBJTriangles(
        const std::string& filepath,
        const Material& material,
        const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f),
        const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f),
        bool smoothNormals = true
    );

private:
    // Estructura interna para guardar información de vértice
    struct VertexData {
        int posIndex;
        int normalIndex;
        int texIndex; // No lo usaremos pero es parte del formato
    };

    // Parse una línea de vértice (v)
    static bool parseVertex(const std::string& line, Vector3& vertex);
    
    // Parse una línea de normal (vn)
    static bool parseNormal(const std::string& line, Vector3& normal);
    
    // Parse una línea de cara (f)
    static bool parseFace(const std::string& line, std::vector<VertexData>& faceVertices);
    
    // Convierte índices negativos a positivos
    static int resolveIndex(int index, int size);
};
