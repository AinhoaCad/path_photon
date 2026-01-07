#include "OBJLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

bool OBJLoader::loadOBJ(
    const std::string& filepath,
    Scene& scene,
    const Material& material,
    const Vector3& scale,
    const Vector3& translation,
    bool smoothNormals,
    float trianglePercentage
) {
    std::vector<Triangle> triangles = loadOBJTriangles(filepath, material, scale, translation, smoothNormals, trianglePercentage);
    
    if (triangles.empty()) {
        return false;
    }

    // Añadir todos los triángulos a la escena
    for (const auto& tri : triangles) {
        scene.addTriangle(tri);
    }

    std::cout << "[OBJ] Cargados " << triangles.size() << " triángulos desde " << filepath 
              << " (" << trianglePercentage << "% de la malla)" << std::endl;
    return true;
}

std::vector<Triangle> OBJLoader::loadOBJTriangles(
    const std::string& filepath,
    const Material& material,
    const Vector3& scale,
    const Vector3& translation,
    bool smoothNormals,
    float trianglePercentage
) {
    // Vector donde se almacenarán los triángulos finales que se devolverán
    std::vector<Triangle> triangles;

    // Vector de vértices cargados desde el OBJ
    std::vector<Vector3> vertices;

    // Vector de normales cargadas desde el OBJ (vn). Si no hay, se usará normal geométrica
    std::vector<Vector3> normals;

    // Vector auxiliar para calcular normales suavizadas si no hay vn en el OBJ
    std::vector<Vector3> vertexNormals;

    // Abrir el archivo OBJ
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[OBJ] Error: No se pudo abrir el archivo " << filepath << std::endl;
        return triangles;
    }

    // Validación del porcentaje de triángulos que queremos cargar
    if (trianglePercentage <= 0.0f || trianglePercentage > 100.0f) {
        std::cerr << "[OBJ] Advertencia: Porcentaje inválido " << trianglePercentage 
                  << "%. Usando 100%." << std::endl;
        trianglePercentage = 100.0f;
    }

    std::string line;
    int lineNumber = 0;

    // Estructura auxiliar para almacenar las caras mientras leemos el OBJ
    struct FaceIndices {
        std::vector<int> posIndices;    // Índices de vértices
        std::vector<int> normalIndices; // Índices de normales (puede estar vacío)
    };
    std::vector<FaceIndices> faces; // Todas las caras del OBJ

    // ===========================================
    // 1. Lectura de vértices, normales y caras
    // ===========================================
    while (std::getline(file, line)) {
        lineNumber++;

        // Saltar líneas vacías o comentarios
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == std::string::npos || line[firstNonSpace] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Línea de vértice: "v x y z"
            Vector3 vertex;
            if (parseVertex(line, vertex)) {
                // Aplicar escala y traslación a los vértices
                vertex.x = vertex.x * scale.x + translation.x;
                vertex.y = vertex.y * scale.y + translation.y;
                vertex.z = vertex.z * scale.z + translation.z;
                vertices.push_back(vertex);
            }
        } 
        else if (prefix == "vn") {
            // Línea de normal: "vn x y z"
            Vector3 normal;
            if (parseNormal(line, normal)) {
                normals.push_back(normal.normalized()); // Normalizar para seguridad
            }
        } 
        else if (prefix == "f") {
            // Línea de cara: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ..."
            std::vector<VertexData> faceVertices;
            if (!parseFace(line, faceVertices) || faceVertices.size() < 3) continue;

            FaceIndices f;
            for (auto& vd : faceVertices) {
                f.posIndices.push_back(resolveIndex(vd.posIndex, vertices.size()));
                f.normalIndices.push_back(resolveIndex(vd.normalIndex, normals.size()));
            }
            faces.push_back(f);
        }
    }
    file.close();

    // ===========================================
    // 2. Preparación de normales de vértices si no hay vn y queremos smooth
    // ===========================================
    if (smoothNormals && normals.empty()) {
        // Inicializamos vector de normales por vértice a cero
        vertexNormals.resize(vertices.size(), Vector3(0,0,0));
    }

    // ===========================================
    // 3. Procesamiento de caras y cálculo de normales
    // ===========================================
    for (auto& f : faces) {
        // Triangulamos polígonos usando "fan triangulation"
        for (size_t i = 1; i < f.posIndices.size() - 1; ++i) {
            int idx0 = f.posIndices[0];
            int idx1 = f.posIndices[i];
            int idx2 = f.posIndices[i + 1];

            Point v0 = vertices[idx0].toPoint();
            Point v1 = vertices[idx1].toPoint();
            Point v2 = vertices[idx2].toPoint();

            // Calcular la normal geométrica de la cara
            Vector3 e1 = v1 - v0;
            Vector3 e2 = v2 - v0;
            Direction faceNormal = e1.cross(e2).normalized().toDirection();

            if (!smoothNormals) {
                // Flat shading: todos los vértices del triángulo comparten la misma normal
                triangles.emplace_back(v0, v1, v2, faceNormal, faceNormal, faceNormal, material, false);
            } 
            else if (!normals.empty() && f.normalIndices[0] != -1) {
                // Smooth shading usando normales del archivo (vn)
                Direction n0 = normals[f.normalIndices[0]].toDirection();
                Direction n1 = normals[f.normalIndices[i]].toDirection();
                Direction n2 = normals[f.normalIndices[i + 1]].toDirection();
                triangles.emplace_back(v0, v1, v2, n0, n1, n2, material, true);
            } 
            else {
                // Smooth shading manual: acumulamos normales por vértice
                vertexNormals[idx0] += faceNormal.toVector();
                vertexNormals[idx1] += faceNormal.toVector();
                vertexNormals[idx2] += faceNormal.toVector();
            }
        }
    }

    // ===========================================
    // 4. Si usamos suavizado manual, normalizamos y reconstruimos triángulos
    // ===========================================
    if (smoothNormals && normals.empty()) {
        for (auto& vn : vertexNormals) vn = vn.normalized();

        triangles.clear(); // reconstruimos triángulos con normales suavizadas
        for (auto& f : faces) {
            for (size_t i = 1; i < f.posIndices.size() - 1; ++i) {
                int idx0 = f.posIndices[0];
                int idx1 = f.posIndices[i];
                int idx2 = f.posIndices[i + 1];

                Point v0 = vertices[idx0].toPoint();
                Point v1 = vertices[idx1].toPoint();
                Point v2 = vertices[idx2].toPoint();

                Direction n0 = vertexNormals[idx0].toDirection();
                Direction n1 = vertexNormals[idx1].toDirection();
                Direction n2 = vertexNormals[idx2].toDirection();

                triangles.emplace_back(v0, v1, v2, n0, n1, n2, material, true);
            }
        }
    }

    // ===========================================
    // 5. Aplicar filtro de porcentaje de triángulos
    // ===========================================
    if (trianglePercentage < 100.0f && !triangles.empty()) {
        size_t targetSize = (size_t)(triangles.size() * trianglePercentage / 100.0f);
        triangles.erase(triangles.begin() + targetSize, triangles.end());
        std::cout << "[OBJ] Reducido a " << triangles.size() << " triángulos (" 
                  << trianglePercentage << "%)" << std::endl;
    }

    // Advertencia si no se cargó ningún triángulo
    if (triangles.empty()) {
        std::cerr << "[OBJ] Advertencia: No se encontraron triángulos en " << filepath << std::endl;
    }

    return triangles;
}


bool OBJLoader::parseVertex(const std::string& line, Vector3& vertex) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix; // "v"
    
    if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
        return false;
    }
    return true;
}

bool OBJLoader::parseNormal(const std::string& line, Vector3& normal) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix; // "vn"
    
    if (!(iss >> normal.x >> normal.y >> normal.z)) {
        return false;
    }
    return true;
}

bool OBJLoader::parseFace(const std::string& line, std::vector<VertexData>& faceVertices) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix; // "f"

    std::string vertexStr;
    while (iss >> vertexStr) {
        VertexData vd;
        vd.posIndex = -1;
        vd.normalIndex = -1;
        vd.texIndex = -1;

        // Formato: v, v/vt, v/vt/vn, v//vn
        size_t slash1 = vertexStr.find('/');
        if (slash1 == std::string::npos) {
            // Solo índice de vértice: "1"
            vd.posIndex = std::stoi(vertexStr);
        } else {
            // Tiene al menos una barra
            vd.posIndex = std::stoi(vertexStr.substr(0, slash1));
            
            size_t slash2 = vertexStr.find('/', slash1 + 1);
            if (slash2 == std::string::npos) {
                // Formato v/vt
                if (slash1 + 1 < vertexStr.size()) {
                    vd.texIndex = std::stoi(vertexStr.substr(slash1 + 1));
                }
            } else {
                // Formato v/vt/vn o v//vn
                if (slash2 > slash1 + 1) {
                    vd.texIndex = std::stoi(vertexStr.substr(slash1 + 1, slash2 - slash1 - 1));
                }
                if (slash2 + 1 < vertexStr.size()) {
                    vd.normalIndex = std::stoi(vertexStr.substr(slash2 + 1));
                }
            }
        }

        faceVertices.push_back(vd);
    }

    return !faceVertices.empty();
}

int OBJLoader::resolveIndex(int index, int size) {
    if (index > 0) {
        return index - 1; // OBJ usa índices base 1
    } else if (index < 0) {
        return size + index; // Índices negativos cuentan desde el final
    }
    return -1; // 0 es inválido en OBJ
}
