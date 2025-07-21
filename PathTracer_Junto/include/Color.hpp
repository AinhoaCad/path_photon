#ifndef COLOR_HPP
#define COLOR_HPP

class Color {
public:
    float R, G, B;  // Componentes del color (rojo, verde, azul)

    Color() : R(0), G(0), B(0) {}  // Constructor por defecto
    Color(float r, float g, float b) : R(r), G(g), B(b) {}  // Constructor con valores

    // Métodos para manipular los colores
    Color operator+(const Color& other) const;
    Color operator-(const Color& other) const;
    Color operator*(float scalar) const;
    Color operator*(const Color& other) const;
    Color& operator+=(const Color& other);
    Color& operator-=(const Color& other);
    Color& operator*=(float scalar);
    Color operator/(float scalar) const;
};

#endif // COLOR_HPP