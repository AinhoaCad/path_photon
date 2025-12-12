#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include "RGB.hpp"


using namespace std;


class Image {


    public:
    std::vector<RGB> RGBes;
    int width;
    int height;

    Image(){};

    // Constructors
    Image(std::vector<RGB> _RGBes, int _width, int _height)
        : RGBes(_RGBes), width(_width), height(_height){};

    Image(int _width, int _height) : width(_width), height(_height) {
        RGB p(0,0,0);
        RGBes = vector<RGB>(width * height, p);
    };

    void applyToneMappingOperator(string ToneMappingType,float m);
    float max();
};

#endif
