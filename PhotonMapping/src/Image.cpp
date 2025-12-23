#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <filesystem>
// TinyEXR (header-only) + bundled miniz. Ensure include/third_party/{tinyexr.h,miniz.h} exist.
#define TINYEXR_IMPLEMENTATION
#define TINYEXR_USE_MINIZ 1
#ifndef _strdup
// Provide _strdup for compilers where it's missing
#define _strdup strdup
#endif
#ifndef strnlen
static size_t strnlen_local(const char* s, size_t max) { size_t n = 0; while (n < max && s[n] != '\0') ++n; return n; }
#define strnlen strnlen_local
#endif
#ifndef strdup
static char* strdup_local(const char* s) {
    if (!s) return nullptr;
    size_t len = std::strlen(s) + 1;
    char* p = static_cast<char*>(std::malloc(len));
    if (!p) return nullptr;
    std::memcpy(p, s, len);
    return p;
}
#define strdup strdup_local
#ifndef _strdup
#define _strdup strdup_local
#endif
#endif
#include "third_party/miniz.h"
#include "third_party/tinyexr.h"
#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

// Constructor por defecto
Image::Image() {
}

// Constructor con ancho y alto
Image::Image(int w, int h) : width(w), height(h) {
    pixels = new Pixel[width * height];
}

// Destructor
Image::~Image() {
    delete[] pixels;
}

// Establece un píxel en una posición específica (x, y)
void Image::setPixel(int x, int y, Pixel p) {
    // Verifica que la posición esté dentro del rango de la imagen
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = p;
    }
}

// Obtiene un píxel en una posición específica (x, y)
Pixel Image::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y * width + x];
    }
    // Si está fuera de los límites, retorna un píxel negro
    return {0, 0, 0};
}

// Obtiene el ancho de la imagen
int Image::getWidth() const {
    return width;
}

// Obtiene la altura de la imagen
int Image::getHeight() const {
    return height;
}
bool Image::readPNG(const std::string& filename) {
    int w = 0, h = 0, n = 0;
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 3);
    if (!data) {
        std::cerr << "Error: no se pudo leer PNG: " << filename << "\n";
        return false;
    }
    width = w;
    height = h;
    pixels = new Pixel[width * height];
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            size_t idx = (static_cast<size_t>(y) * width + x) * 3;
            Pixel p;
            p.r = data[idx + 0] / 255.0f;
            p.g = data[idx + 1] / 255.0f;
            p.b = data[idx + 2] / 255.0f;
            setPixel(x, y, p);
        }
    }
    stbi_image_free(data);
    return true;
}
bool Image::writePNG(const std::string& filename) const {
    // Convertir a RGB8
    std::vector<unsigned char> rgb;
    rgb.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 3);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            size_t idx = (static_cast<size_t>(y) * width + x) * 3;
            Pixel p = getPixel(x, y);
            rgb[idx + 0] = (unsigned char)(std::clamp(p.r, 0.0f, 1.0f) * 255.0f);
            rgb[idx + 1] = (unsigned char)(std::clamp(p.g, 0.0f, 1.0f) * 255.0f);
            rgb[idx + 2] = (unsigned char)(std::clamp(p.b, 0.0f, 1.0f) * 255.0f);
        }
    }

    size_t out_len = 0;
    void* out_png = tdefl_write_image_to_png_file_in_memory_ex(rgb.data(), width, height, 3, &out_len, MZ_DEFAULT_LEVEL, 0);
    if (!out_png || out_len == 0) {
        std::cerr << "Error al codificar PNG en memoria: " << filename << "\n";
        if (out_png) mz_free(out_png);
        return false;
    }

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: no se pudo abrir PNG para escritura: " << filename << "\n";
        mz_free(out_png);
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(out_png), static_cast<std::streamsize>(out_len));
    ofs.close();
    mz_free(out_png);
    return true;
}


// Escribe la imagen en un archivo en formato PPM
bool Image::writePPM(std::string filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura: " << filename << std::endl;
        return false;
    }

     // Escribe la cabecera del archivo PPM
    file << "P3\n" << width << " " << height << "\n255\n";

    // Escribe cada píxel como valores RGB enteros (0–255)
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Pixel p = getPixel(i, j);
            file << static_cast<int>(p.r * 255) << " "
                 << static_cast<int>(p.g * 255) << " "
                 << static_cast<int>(p.b * 255) << " ";    
        }
        file << "\n";
    }

    file.close();
    return true;
}

// Lee una imagen desde un archivo PPM
bool Image::readPPM(std::string filename) {
    printf("Comienza a leer\n");
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para lectura: " << filename << std::endl;
        return false;
    }

    std::string line;
    int maxColorValue = 255;
    float optionalMaxColorValue = 1.0;
    float maxColor = 0.0;
    
    // Lee el identificador del archivo
    if (!std::getline(file, line) || line != "P3") {
        std::cerr << "Error: Formato no soportado. Solo se soporta el formato P3." << std::endl;
        return false;
    }
    
    
     // Lee comentarios y dimensiones
    while (std::getline(file, line)) {
        if (line[0] == '#') {
            // Si es un comentario especial #MAX=, se guarda el valor
            if (line.find("#MAX=") == 0) {
                std::istringstream maxStream(line.substr(5));
                maxStream >> optionalMaxColorValue;
            }
            continue;   //Ignora otros comentarios
        }
        // Si no es comentario, debe ser la línea con el ancho y alto
        std::istringstream dimStream(line);
        dimStream >> width >> height;
        break;
    }
    // Lectura del maximo valor de color
    while (std::getline(file, line)) {
        if (line[0] == '#') continue;  // Ignore comments

        std::istringstream maxStream(line);
        maxStream >> maxColorValue;
        break;
    }
    // Asignacion de memoria a pixeles
    printf("optionalMaxColorValue: %f\n", optionalMaxColorValue);
    pixels = new Pixel[width * height];
    maxColor = maxColorValue / optionalMaxColorValue;
    printf("maxColor: %f\n", maxColor);
    // Lectura de valores de píxel
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int r, g, b;
            file >> r >> g >> b;
            setPixel(j, i, Pixel{r / maxColor,
                                 g / maxColor,
                                 b / maxColor});
        }
    }

    file.close();
    return true;
}
 

// ============================================================================
// MÉTODOS DE LECTURA/ESCRITURA GENÉRICOS
// ============================================================================

bool Image::read(const std::string& filename) {
    // Detectar formato por extensión
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos) {
        std::cerr << "Error: No se puede determinar el formato del archivo: " << filename << std::endl;
        return false;
    }
    
    std::string ext = filename.substr(dotPos);
    
    // Convertir a minúsculas
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".ppm") {
        return readPPM(filename);
    } else if (ext == ".pfm") {
        return readPFM(filename);
    } else if (ext == ".exr") {
        return readEXR(filename);
    } else if (ext == ".png") {
        return readPNG(filename);
    } else {
        std::cerr << "Error: Formato no soportado: " << ext << std::endl;
        return false;
    }
}

bool Image::write(const std::string& filename) const {
    // Detectar formato por extensión
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos) {
        std::cerr << "Error: No se puede determinar el formato del archivo: " << filename << std::endl;
        return false;
    }
    
    std::string ext = filename.substr(dotPos);
    
    // Convertir a minúsculas
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".ppm") {
        return writePPM(filename);
    } else if (ext == ".pfm") {
        return writePFM(filename);
    } else if (ext == ".exr") {
        return writeEXR(filename);
    } else if (ext == ".png") {
        return writePNG(filename);
    } else {
        std::cerr << "Error: Formato no soportado: " << ext << std::endl;
        return false;
    }
}

// ============================================================================
// SOPORTE PARA PFM (High Dynamic Range ASCII)
// ============================================================================

bool Image::writePFM(std::string filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura: " << filename << std::endl;
        return false;
    }

    // Escribir cabecera PFM
    file << "PF\n";  // PF para RGB, Pf para escala de grises
    file << width << " " << height << "\n";
    file << "-1.0\n";  // -1.0 para little-endian

    // Escribir datos en binario (float de 32 bits)
    for (int j = height - 1; j >= 0; --j) {  // PFM usa orden invertido Y
        for (int i = 0; i < width; ++i) {
            Pixel p = getPixel(i, j);
            file.write((char*)&p.r, sizeof(float));
            file.write((char*)&p.g, sizeof(float));
            file.write((char*)&p.b, sizeof(float));
        }
    }

    file.close();
    return true;
}

bool Image::readPFM(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para lectura: " << filename << std::endl;
        return false;
    }

    std::string magic;
    file >> magic;
    
    if (magic != "PF" && magic != "Pf") {
        std::cerr << "Error: Formato PFM no válido: " << magic << std::endl;
        return false;
    }

    file >> width >> height;
    
    float endian;
    file >> endian;
    
    // Saltar el newline
    file.ignore();

    pixels = new Pixel[width * height];

    // Leer datos en orden inverso
    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            float r, g, b;
            file.read((char*)&r, sizeof(float));
            file.read((char*)&g, sizeof(float));
            file.read((char*)&b, sizeof(float));
            setPixel(i, j, Pixel{r, g, b});
        }
    }

    file.close();
    return true;
}

// ============================================================================
// SOPORTE PARA EXR (TinyEXR)
// ============================================================================

bool Image::writeEXR(std::string filename) const {
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Dimensiones de imagen invalidas al escribir EXR" << std::endl;
        return false;
    }

    // Construir buffer intercalado RGB con valores HDR originales
    std::vector<float> data;
    data.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 3);
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            size_t idx = (static_cast<size_t>(j) * width + i) * 3;
            Pixel p = getPixel(i, j);
            data[idx + 0] = p.r;
            data[idx + 1] = p.g;
            data[idx + 2] = p.b;
        }
    }

    const char* err = nullptr;
    int ret = SaveEXR(data.data(), width, height, 3 /*channels*/, 0 /*save_as_fp16*/, filename.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Error al escribir EXR: " << (err ? err : "desconocido") << std::endl;
        if (err) FreeEXRErrorMessage(err);
        return false;
    }
    if (err) FreeEXRErrorMessage(err);
    return true;
}

bool Image::readEXR(std::string filename) {
    float* out = nullptr; // RGBA intercalado
    int w = 0, h = 0;
    const char* err = nullptr;
    int ret = LoadEXR(&out, &w, &h, filename.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Error al leer EXR: " << (err ? err : "desconocido") << std::endl;
        if (err) FreeEXRErrorMessage(err);
        return false;
    }
    if (err) FreeEXRErrorMessage(err);

    width = w;
    height = h;
    pixels = new Pixel[width * height];

    // TinyEXR retorna RGBA; copiamos RGB con stride 4
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            size_t idx = (static_cast<size_t>(j) * width + i) * 4;
            setPixel(i, j, Pixel{out[idx + 0], out[idx + 1], out[idx + 2]});
        }
    }

    free(out);
    return true;
}