#include "Material.hpp"
#include "Color.hpp"

    // Colores base
    Color white(1.0f, 1.0f, 1.0f);
    Color black(0.0f, 0.0f, 0.0f);
    Color red(1.0f, 0.0f, 0.0f);
    Color green(0.0f, 1.0f, 0.0f);
    Color blue(0.0f, 0.0f, 1.0f);
    Color purple(0.5f, 0.0f, 0.5f);
    Color gray(0.5f, 0.5f, 0.5f);

    // Materiales difusos básicos
    Material whiteDiffuse  = Material(white, black, black, black, 1.0f);
    Material redDiffuse    = Material(red,   black, black, black, 1.0f);
    Material greenDiffuse  = Material(green, black, black, black, 1.0f);
    Material blueDiffuse   = Material(blue,  black, black, black, 1.0f);
    Material purpleDiffuse = Material(purple, black, black, black, 1.0f);
    Material grayDiffuse   = Material(gray, black, black, black, 1.0f);

    // Material espejo
    Material mirrorSpecular = Material(black, white, black, black, 1.0f);

    // Material dieléctrico (como vidrio)
    Material glassDielectric = Material(black, Color(0.8f, 0.8f, 0.8f), Color(0.8f, 0.8f, 0.8f), black, 1.5f);