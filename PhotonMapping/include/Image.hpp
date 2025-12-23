#pragma once

#include <string>

struct Pixel {
    float r;
    float g;
    float b;
};

// Enum para los diferentes formatos de imagen
enum class ImageFormat {
    PPM,    // Low Dynamic Range (ASCII)
    PFM,    // High Dynamic Range (ASCII)
    EXR,    // High Dynamic Range (OpenEXR)
    PNG     // Low Dynamic Range (binary)
};

// Enum para los operadores de tone mapping
enum class ToneMappingOperator {
    CLAMPING,           // Clamp values to [0, 1]
    EQUALIZATION,       // Linear normalization
    EQUALIZATION_CLAMP, // Equalization + clamping with parameter
    GAMMA,              // Gamma curve (requires equalization)
    GAMMA_CLAMP,        // Gamma curve after clamping
    REINHARD,           // Exposure-based tone mapping
    NONE                // No tone mapping (raw values)
};

class Image {
    private:
        int width;
        int height;
        Pixel* pixels;
        
    public:
        Image();
        Image(int width, int height);
        ~Image();

        void setPixel(int x, int y, Pixel p);
        Pixel getPixel(int x, int y) const;
        int getWidth() const;
        int getHeight() const;

        // Métodos específicos de formato
        bool read(const std::string& filename);
        bool write(const std::string& filename) const;
        
        // Métodos específicos de formato
        bool writePPM(std::string filename) const;
        bool readPPM(std::string filename);
        bool writePFM(std::string filename) const;
        bool readPFM(std::string filename);
        bool writeEXR(std::string filename) const;
        bool readEXR(std::string filename);
        bool writePNG(const std::string& filename) const;
        bool readPNG(const std::string& filename);
};