#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
//#include <png.h>
#include "Image.hpp"



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

/*bool Image::readPNG(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        std::cerr << "Error: no se pudo abrir PNG: " << filename << "\n";
        return false;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return false;

    png_infop info = png_create_info_struct(png);
    if (!info) return false;

    if (setjmp(png_jmpbuf(png))) return false;

    png_init_io(png, fp);
    png_read_info(png, info);

    width  = png_get_image_width(png, info);
    height = png_get_image_height(png, info);

    int color_type = png_get_color_type(png, info);
    int bit_depth  = png_get_bit_depth(png, info);

    // Normalizar formatos
    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY) png_set_gray_to_rgb(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) png_set_strip_alpha(png);

    png_read_update_info(png, info);

    // Reservar memoria
    pixels = new Pixel[width * height];

    std::vector<png_bytep> rows(height);
    for (int y = 0; y < height; y++)
        rows[y] = (png_bytep)malloc(png_get_rowbytes(png, info));

    png_read_image(png, rows.data());

    // Copiar datos en Pixel
    for (int y = 0; y < height; y++) {
        png_bytep row = rows[y];
        for (int x = 0; x < width; x++) {
            setPixel(x, y, Pixel{
                row[x*3+0] / 255.0f,
                row[x*3+1] / 255.0f,
                row[x*3+2] / 255.0f
            });
        }
        free(rows[y]);
    }

    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);

    return true;
}


bool Image::writePNG(const std::string& filename) const {
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Error: no se pudo abrir PNG para escritura: " << filename << "\n";
        return false;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return false;

    png_infop info = png_create_info_struct(png);
    if (!info) return false;

    if (setjmp(png_jmpbuf(png))) return false;

    png_init_io(png, fp);

    // PNG tipo 8-bit RGB
    png_set_IHDR(
        png, info, width, height,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    // Buffer de lineas
    std::vector<png_bytep> rows(height);
    for (int y = 0; y < height; y++) {
        rows[y] = (png_bytep)malloc(3 * width);
        for (int x = 0; x < width; x++) {
            Pixel p = getPixel(x, y);
            rows[y][3*x+0] = (unsigned char)(std::clamp(p.r, 0.0f, 1.0f) * 255);
            rows[y][3*x+1] = (unsigned char)(std::clamp(p.g, 0.0f, 1.0f) * 255);
            rows[y][3*x+2] = (unsigned char)(std::clamp(p.b, 0.0f, 1.0f) * 255);
        }
    }

    png_write_image(png, rows.data());
    png_write_end(png, NULL);

    // Liberar memoria
    for (int y = 0; y < height; y++) free(rows[y]);
    fclose(fp);

    png_destroy_write_struct(&png, &info);

    return true;
}*/


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


// Asegura que todos los valores RGB estén dentro del rango [0.0, 1.0]
void Image::clampPixels() {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Pixel p = getPixel(j, i);
            p.r = std::max(0.0f, std::min(1.0f, p.r));
            p.g = std::max(0.0f, std::min(1.0f, p.g));
            p.b = std::max(0.0f, std::min(1.0f, p.b));
            setPixel(j, i, p);
        }
    }
}

// Aplica ecualización de color para mejorar contraste
void Image::ecualizacion() {
    float minR = 1.0f, minG = 1.0f, minB = 1.0f;
    float maxR = 0.0f, maxG = 0.0f, maxB = 0.0f;

    // Encontrar mínimos y máximos
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Pixel p = getPixel(j, i);
            minR = std::min(minR, p.r);
            minG = std::min(minG, p.g);
            minB = std::min(minB, p.b);
            maxR = std::max(maxR, p.r);
            maxG = std::max(maxG, p.g);
            maxB = std::max(maxB, p.b);
        }
    }

    // Reescala los valores de color al rango [0, 1]
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Pixel p = getPixel(j, i);
            //Comprobamos que no se divida entre 0
            if (maxR > minR) {
                p.r = (p.r - minR) / (maxR - minR);
            }
            if (maxG > minG) {
                p.g = (p.g - minG) / (maxG - minG);
            }
            if (maxB > minB) {
                p.b = (p.b - minB) / (maxB - minB);
            }
            setPixel(j, i, p);
        }
    }
}

// Aplica corrección gamma a la imagen
void Image::curvaGamma(float gamma) {
    ecualizacion();
    
    // Verifica que gamma sea mayor que cero
    if (gamma <= 0.0f) {
        std::cerr << "Error: El valor de gamma debe ser mayor que 0." << std::endl;
        return;
    }

    // Aplica la curva gamma a cada píxel
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Pixel p = getPixel(j, i);
            p.r = std::pow(p.r, gamma);
            p.g = std::pow(p.g, gamma);
            p.b = std::pow(p.b, gamma);
            setPixel(j, i, p); // Actualizar el píxel
        }
    }
}