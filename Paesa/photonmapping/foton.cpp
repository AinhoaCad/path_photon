#include "foton.h"
#include "escena.h"
#include "aleatorio.h"
#include <cmath>

using namespace std;

ostream& operator<<(ostream& os, const Foton& f) {
    os << "Foton(" << f.posicion << ", " << f.direccion << ", " << f.flujo << ")";
    return os;
}

MapaFotones constructorMapaFotones(std::list<Foton> fotones) {
    return MapaFotones(fotones, PosicionEjeFoton());
}

// operador << para mapa de fotones
ostream& operator<<(ostream& os, const MapaFotones& m) {
    os << "MapaFotones(" << m.elements.size() << " fotones)";
    return os;
}
