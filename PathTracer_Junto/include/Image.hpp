#pragma once

#include <string>

struct Pixel {
    float r;
    float g;
    float b;
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

        bool writePPM(std::string filename) const;
        bool readPPM(std::string filename);

        void clampPixels();
        void ecualizacion();
        void curvaGamma(float gamma);
};