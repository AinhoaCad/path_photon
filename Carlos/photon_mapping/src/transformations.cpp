#include "transformations.hpp"

Matrix Transformation::Transalation(float tx, float ty, float tz) {
    float translation_values[4][4] = {
    {1, 0, 0, tx},
    {0, 1, 0, ty},
    {0, 0, 1, tz},
    {0, 0, 0, 1}
    };
    return translation_values;
}


Matrix Transformation::Scale(float sx, float sy, float sz){
    float scale_values[4][4] = {
    {sx, 0, 0, 0},
    {0, sy, 0, 0},
    {0, 0, sz, 0},
    {0, 0, 0, 1}
    };
    return scale_values;
}


Matrix Transformation::Rotation(char axis, float inclination){
    float rotation_values[4][4]={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    if (axis == 'x') {
        rotation_values[1][1] = cos(inclination);
        rotation_values[1][2] = -sin(inclination);
        rotation_values[2][1] = sin(inclination);
        rotation_values[2][2] = cos(inclination);
    } 
    else if (axis == 'y') {
        rotation_values[0][0] = cos(inclination);
        rotation_values[0][2] = sin(inclination);
        rotation_values[2][0] = -sin(inclination);
        rotation_values[2][2] = cos(inclination);
    } 
    else if (axis == 'z') {
        rotation_values[0][0] = cos(inclination);
        rotation_values[0][1] = -sin(inclination);
        rotation_values[1][0] = sin(inclination);
        rotation_values[1][1] = cos(inclination);
    }else{
        throw runtime_error("Error: no se puede rotar respecto a un eje que no sea x, y o z.");
    }
    
    return rotation_values;
}

Matrix Transformation::ChangeBasis(const Direction& u,const Direction& v, const Direction& w, const Point& o){
    float ChangeBase_values[4][4]={
        {u[0],v[0],w[0], o[0]},
        {u[1],v[1],w[1], o[1]},
        {u[2],v[2],w[2], o[2]},
        {0.0f,0.0f,0.0f,1.0f}
    };
    return ChangeBase_values;
}

Matrix InverseChangeBasis(const Matrix& basis) {
    return basis.inverse();    
}