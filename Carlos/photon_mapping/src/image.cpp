
#include "image.hpp"
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>


using namespace std;


void Image::applyToneMappingOperator(string mappingType,float m){
    
    float gamma,v;
    if (mappingType == "clamping") {
        //gamma=1.0f;
        //v=1.0f;
        gamma=1;
        v=1;
    } else if (mappingType == "equalization") {
        gamma=1.0f;
        v=m;
    } else if (mappingType == "gamma") {
        gamma=0.5f;
        v=m;
    } else if (mappingType == "clamp_gamma") {
        gamma=0.5f;
        //v = c + static_cast<float>(rand()) / RAND_MAX * (m - c);
        v=m*0.75;
    } else if (mappingType == "equalize_clamp") {
        gamma=1.0f;
        //v = c + static_cast<float>(rand()) / RAND_MAX * (m - c);
        v=m/2;
    } else if (mappingType == "reinhard"){
        //https://64.github.io/tonemapping/
        // Aplicar operador de tono Reinhard
        float Lp, max_white_l = 0.0f;
        for (RGB &RGB : RGBes) {
            Lp = RGB.r * 0.2126f + RGB.g * 0.7152f + RGB.b * 0.0722f;
            max_white_l = std::max(max_white_l, Lp);
        }
        
        for (RGB &RGB : RGBes) {
            float luminance = RGB.r * 0.2126f + RGB.g * 0.7152f + RGB.b * 0.0722f;
            float scaled_luminance = luminance * (1.0f + (luminance / (max_white_l * max_white_l))) / (1.0f + luminance);
            
            // Escalar los valores de los colores RGB según el mapeo
            RGB.r = RGB.r / luminance * scaled_luminance;
            RGB.g = RGB.g / luminance * scaled_luminance;
            RGB.b = RGB.b / luminance * scaled_luminance;
        }

    }else if(mappingType == "mantiuk"){
        
    }else {
    }
    if (mappingType != "reinhard"){
        for (RGB &P : RGBes){
            if (P.r <= v){
                P.r= pow(P.r / v, gamma);
            }else{
                P.r=1.0f;
            }
            if (P.g<= v){
                P.g = pow(P.g / v, gamma);
            }else{
                P.g=1.0f;
            }
            if (P.b <=v){
                P.b = pow(P.b / v, gamma);
            }else{
                P.b=1.0f;
            }
        }
    }
}

float Image::max(){
    float newM=0;
    for (RGB &p : RGBes){
        if (p.r>newM){
            newM=p.r;
        }
        if (p.g>newM){
            newM=p.g;
        }
        if (p.b>newM){
            newM=p.b;
        }
    }
    return newM;
}

