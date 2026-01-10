#pragma once

#include "Image.hpp"

namespace Tonemapping {
    // In-place operations on Image
    void clampPixels(Image& img, float maxValue = 1.0f);
    void ecualizacion(Image& img);
    void ecualizacionClamp(Image& img, float v);
    void curvaGamma(Image& img, float gamma);
    void gammaClamp(Image& img, float gamma);

    void applyToneMapping(Image& img, ToneMappingOperator op, float param = 1.0f);

    // Utilities
    float getLuminance(const Pixel& p);
    float getMaxLuminance(const Image& img);
    float getMinLuminance(const Image& img);
}
