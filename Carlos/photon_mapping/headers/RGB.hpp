#ifndef RGB_HPP
#define RGB_HPP

class RGB{
    public:
        float r;
        float g;
        float b;
        RGB(float _r,float _g,float _b);
        RGB();
        friend RGB operator*(const RGB& c, float r);
        friend RGB operator/(float r,const RGB& c);
        friend RGB operator*(const RGB& rgb1, const RGB& rgb2);
        friend RGB operator+(const RGB& rgb1, const RGB& rgb2);
        float operator[] (int coord) const;
};
#endif