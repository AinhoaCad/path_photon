#ifndef PHOTON_HPP
#define PHOTON_HPP

#include "point.hpp"
#include "direction.hpp"
#include "RGB.hpp"

class Photon{
    Point position;
    Direction incidentDir;
    RGB flux;
    public:
        Photon(Point _position,Direction _incidentDir, RGB _flux);
        Point getPosition() const;
        Direction getIncidentDir();
        RGB getFlux() const ;
        float Position(std::size_t i) const {return position[i];};
};


#endif