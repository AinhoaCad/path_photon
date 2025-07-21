#include "Material.hpp"

// Definición de materiales predeterminados
Material Diffuse_White = Material(Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
Material Diffuse_Red = Material(Color(1.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
Material Specular_Mirror = Material(Color(0.0f, 0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
Material Dielectric_Glass = Material(Color(0.0f, 0.0f, 0.0f), Color(0.8f, 0.8f, 0.8f), Color(0.8f, 0.8f, 0.8f), Color(0.0f, 0.0f, 0.0f), 1.5f);
