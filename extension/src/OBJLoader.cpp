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
    bool smoothNormals
) {
    std::vector<Triangle> triangles = loadOBJTriangles(filepath, material, scale, translation, smoothNormals);
    
    if (triangles.empty()) {
        return false;
    }

    // Añadir todos los triángulos a la escena
    for (const auto& tri : triangles) {
        scene.addTriangle(tri);
    }

    std::cout << "[OBJ] Cargados " << triangles.size() << " triángulos desde " << filepath << std::endl;
    return true;
}

std::vector<Triangle> OBJLoader::loadOBJTriangles(
    const std::string& filepath,
    const Material& material,
    const Vector3& scale,
    const Vector3& translation,
    bool smoothNormals
) {
    std::vector<Triangle> triangles;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[OBJ] Error: No se pudo abrir el archivo " << filepath << std::endl;
        return triangles;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        
        // Eliminar espacios al inicio
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == std::string::npos || line[firstNonSpace] == '#') {
            continue; // Línea vacía o comentario
        }

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Vértice
            Vector3 vertex;
            if (parseVertex(line, vertex)) {
                // Aplicar escala y traslación
                vertex.x = vertex.x * scale.x + translation.x;
                vertex.y = vertex.y * scale.y + translation.y;
                vertex.z = vertex.z * scale.z + translation.z;
                vertices.push_back(vertex);
            }
        }
        else if (prefix == "vn") {
            // Normal
            Vector3 normal;
            if (parseNormal(line, normal)) {
                normals.push_back(normal.normalized());
            }
        }
        else if (prefix == "f") {
            // Cara
            std::vector<VertexData> faceVertices;
            if (parseFace(line, faceVertices)) {
                // Un OBJ puede tener caras con más de 3 vértices (polígonos)
                // Triangulamos usando fan triangulation
                if (faceVertices.size() < 3) {
                    std::cerr << "[OBJ] Advertencia: Cara con menos de 3 vértices en línea " 
                              << lineNumber << std::endl;
                    continue;
                }

                // Crear triángulos desde el polígono (fan triangulation)
                for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                    int idx0 = resolveIndex(faceVertices[0].posIndex, vertices.size());
                    int idx1 = resolveIndex(faceVertices[i].posIndex, vertices.size());
                    int idx2 = resolveIndex(faceVertices[i + 1].posIndex, vertices.size());

                    if (idx0 < 0 || idx0 >= (int)vertices.size() ||
                        idx1 < 0 || idx1 >= (int)vertices.size() ||
                        idx2 < 0 || idx2 >= (int)vertices.size()) {
                        std::cerr << "[OBJ] Advertencia: Índices de vértice fuera de rango en línea " 
                                  << lineNumber << std::endl;
                        continue;
                    }

                    Point v0 = vertices[idx0].toPoint();
                    Point v1 = vertices[idx1].toPoint();
                    Point v2 = vertices[idx2].toPoint();

                    // Normales
                    Direction n0, n1, n2;
                    
                    // En modo flat, todos los vértices usan la misma normal geométrica
                    if (!smoothNormals) {
                        Vector3 e1 = v1 - v0;
                        Vector3 e2 = v2 - v0;
                        Direction normal = e1.cross(e2).normalized().toDirection();
                        n0 = n1 = n2 = normal;
                    }
                    // En modo smooth, intentar usar normales del archivo
                    else if (faceVertices[0].normalIndex != -1 && !normals.empty()) {
                        int nidx0 = resolveIndex(faceVertices[0].normalIndex, normals.size());
                        int nidx1 = resolveIndex(faceVertices[i].normalIndex, normals.size());
                        int nidx2 = resolveIndex(faceVertices[i + 1].normalIndex, normals.size());

                        if (nidx0 >= 0 && nidx0 < (int)normals.size() &&
                            nidx1 >= 0 && nidx1 < (int)normals.size() &&
                            nidx2 >= 0 && nidx2 < (int)normals.size()) {
                            n0 = normals[nidx0].toDirection();
                            n1 = normals[nidx1].toDirection();
                            n2 = normals[nidx2].toDirection();
                        } else {
                            // Fallback a normal geométrica
                            Vector3 e1 = v1 - v0;
                            Vector3 e2 = v2 - v0;
                            Direction normal = e1.cross(e2).normalized().toDirection();
                            n0 = n1 = n2 = normal;
                        }
                    } else {
                        // Sin normales en archivo, usar normal geométrica
                        Vector3 e1 = v1 - v0;
                        Vector3 e2 = v2 - v0;
                        Direction normal = e1.cross(e2).normalized().toDirection();
                        n0 = n1 = n2 = normal;
                    }

                    triangles.emplace_back(v0, v1, v2, n0, n1, n2, material, smoothNormals);
                }
            }
        }
        // Ignorar otras líneas (vt, mtllib, usemtl, etc.)
    }

    file.close();

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
