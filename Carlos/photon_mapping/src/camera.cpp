#include "camera.hpp"

Camera::Camera(Point _o, Direction _l,Direction _u, Direction _f): o(_o),u(_u),l(_l),f(_f)
{
}

Point Camera::getOrigin()
{
    return o;
}

Direction Camera::getu()
{
    return u;
}

Direction Camera::getl()
{
    return l;
}

Direction Camera::getf()
{
    return f;
}
