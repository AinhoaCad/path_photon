#include "RGB.hpp"
    
RGB::RGB(float _r,float _g,float _b): r(_r),g(_g),b(_b){
    
}


RGB operator*(const RGB& D, float r) {
    return RGB(r * D.r, r * D.g, r * D.b);
}

RGB operator/(float r, const RGB &D)
{
    return RGB(r/D.r,r/D.g,r/D.b);
}

RGB operator*(const RGB& rgb1, const RGB& rgb2) {
    return RGB(rgb1.r * rgb2.r, rgb1.g * rgb2.g, rgb1.b * rgb2.b);
}

RGB operator+(const RGB& rgb1, const RGB& rgb2) {
    return RGB(rgb1.r + rgb2.r, rgb1.g + rgb2.g, rgb1.b + rgb2.b);
}

RGB::RGB():r(0),g(0),b(0)
{
}

float RGB::operator[](int coord) const
{
    if (coord ==0){
        return r;
    }else if (coord ==1){
        return g;
    }else{
        return b;
    }
}
