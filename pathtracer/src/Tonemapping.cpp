#include <algorithm>
#include <cmath>
#include "Tonemapping.hpp"

namespace Tonemapping {

void clampPixels(Image& img, float maxValue) {
    const int h = img.getHeight();
    const int w = img.getWidth();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            p.r = std::max(0.0f, std::min(maxValue, p.r));
            p.g = std::max(0.0f, std::min(maxValue, p.g));
            p.b = std::max(0.0f, std::min(maxValue, p.b));
            img.setPixel(j, i, p);
        }
    }
}

void ecualizacion(Image& img) {
    const int h = img.getHeight();
    const int w = img.getWidth();

    float minR = 1.0f, minG = 1.0f, minB = 1.0f;
    float maxR = 0.0f, maxG = 0.0f, maxB = 0.0f;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            minR = std::min(minR, p.r);
            minG = std::min(minG, p.g);
            minB = std::min(minB, p.b);
            maxR = std::max(maxR, p.r);
            maxG = std::max(maxG, p.g);
            maxB = std::max(maxB, p.b);
        }
    }

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            if (maxR > minR) p.r = (p.r - minR) / (maxR - minR);
            if (maxG > minG) p.g = (p.g - minG) / (maxG - minG);
            if (maxB > minB) p.b = (p.b - minB) / (maxB - minB);
            img.setPixel(j, i, p);
        }
    }
}

void ecualizacionClamp(Image& img, float v) {
    const int h = img.getHeight();
    const int w = img.getWidth();

    float minR = 1e9f, minG = 1e9f, minB = 1e9f;
    float maxR = -1e9f, maxG = -1e9f, maxB = -1e9f;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            minR = std::min(minR, p.r);
            minG = std::min(minG, p.g);
            minB = std::min(minB, p.b);
            maxR = std::max(maxR, p.r);
            maxG = std::max(maxG, p.g);
            maxB = std::max(maxB, p.b);
        }
    }

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            if (maxR > minR) p.r = (p.r - minR) / (maxR - minR); else p.r = 0.0f;
            if (maxG > minG) p.g = (p.g - minG) / (maxG - minG); else p.g = 0.0f;
            if (maxB > minB) p.b = (p.b - minB) / (maxB - minB); else p.b = 0.0f;
            p.r = std::min(p.r, v);
            p.g = std::min(p.g, v);
            p.b = std::min(p.b, v);
            img.setPixel(j, i, p);
        }
    }
}

void curvaGamma(Image& img, float gamma) {
    ecualizacion(img);
    if (gamma <= 0.0f) return;
    const int h = img.getHeight();
    const int w = img.getWidth();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            p.r = std::pow(p.r, gamma);
            p.g = std::pow(p.g, gamma);
            p.b = std::pow(p.b, gamma);
            img.setPixel(j, i, p);
        }
    }
}

void gammaClamp(Image& img, float gamma) {
    ecualizacion(img);
    if (gamma <= 0.0f) return;
    clampPixels(img, 1.0f);
    const int h = img.getHeight();
    const int w = img.getWidth();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            p.r = std::pow(p.r, gamma);
            p.g = std::pow(p.g, gamma);
            p.b = std::pow(p.b, gamma);
            img.setPixel(j, i, p);
        }
    }
}

void applyToneMapping(Image& img, ToneMappingOperator op, float param) {
    switch (op) {
        case ToneMappingOperator::CLAMPING:
            clampPixels(img, 1.0f);
            break;
        case ToneMappingOperator::EQUALIZATION:
            ecualizacion(img);
            break;
        case ToneMappingOperator::EQUALIZATION_CLAMP:
            ecualizacionClamp(img, param);
            break;
        case ToneMappingOperator::GAMMA:
            curvaGamma(img, param);
            break;
        case ToneMappingOperator::GAMMA_CLAMP:
            gammaClamp(img, param);
            break;
        case ToneMappingOperator::REINHARD:
            {
                const int h = img.getHeight();
                const int w = img.getWidth();
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        Pixel p = img.getPixel(j, i);
                        p.r = std::pow(std::clamp(p.r * param, 0.0f, 1.0f), 1.0f / 2.2f);
                        p.g = std::pow(std::clamp(p.g * param, 0.0f, 1.0f), 1.0f / 2.2f);
                        p.b = std::pow(std::clamp(p.b * param, 0.0f, 1.0f), 1.0f / 2.2f);
                        img.setPixel(j, i, p);
                    }
                }
            }
            break;
        case ToneMappingOperator::NONE:
            break;
    }
}

float getLuminance(const Pixel& p) {
    return 0.2126f * p.r + 0.7152f * p.g + 0.0722f * p.b;
}

float getMaxLuminance(const Image& img) {
    float maxLum = 0.0f;
    const int h = img.getHeight();
    const int w = img.getWidth();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            maxLum = std::max(maxLum, getLuminance(p));
        }
    }
    return maxLum;
}

float getMinLuminance(const Image& img) {
    float minLum = 1e9f;
    const int h = img.getHeight();
    const int w = img.getWidth();
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            Pixel p = img.getPixel(j, i);
            minLum = std::min(minLum, getLuminance(p));
        }
    }
    return minLum == 1e9f ? 0.0f : minLum;
}

} // namespace Tonemapping
