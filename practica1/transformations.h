#include "matrix.h"
#include "point.h"
#include "direction.h"

class transformations {
public:
    static Matrix translation(float tx, float ty, float tz);
    static Matrix rotationX(float angle);
    static Matrix rotationY(float angle);
    static Matrix rotationZ(float angle);
    static Matrix scale(float sx, float sy, float sz);
    static Matrix changeBase(Direction u, Direction v, Direction w, Point origin);
    static Matrix inverseTranslation(float tx, float ty, float tz);
    static Matrix inverseScale(float sx, float sy, float sz);
};
