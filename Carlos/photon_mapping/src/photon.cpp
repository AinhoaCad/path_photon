#include "photon.hpp"

Photon::Photon(Point _position, Direction _incidentDir, RGB _flux): position(_position),incidentDir(_incidentDir),flux(_flux)
{
}

Point Photon::getPosition() const
{
    return position;
}

Direction Photon::getIncidentDir()
{
    return incidentDir;
}

RGB Photon::getFlux() const
{
    return flux;
}
