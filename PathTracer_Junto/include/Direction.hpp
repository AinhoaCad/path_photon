#ifndef DIRECTION_HPP
#define DIRECTION_HPP

#include "Vector3.hpp"

class Direction {
public:
    float x, y, z;

    // Constructores
    Direction();
    Direction(float x, float y, float z);
    Direction(const Vector3& v);

    Vector3 toVector() const;
    Direction normalized() const;

    Direction operator*(float scalar) const;
    Direction operator/(float scalar) const;

    float length() const;

    friend std::ostream& operator<<(std::ostream& os, const Direction& d);
};

#endif // DIRECTION_HPP
