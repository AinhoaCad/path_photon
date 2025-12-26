#include "Material.hpp"
#include "Color.hpp"

// Colores base
static Color white(1.0f, 1.0f, 1.0f);
static Color black(0.0f, 0.0f, 0.0f);
static Color red(1.0f, 0.0f, 0.0f);
static Color green(0.0f, 1.0f, 0.0f);
static Color blue(0.0f, 0.0f, 1.0f);
static Color cyan(0.5f, 0.9f, 0.9f);
static Color purple(0.8f, 0.6f, 0.9f);
static Color gray(0.5f, 0.5f, 0.5f);

// Helper: normalizar kd+ks+kt por canal si la suma excede 1
static void normalizeColors(Color& kd, Color& ks, Color& kt) {
    float sumR = kd.R + ks.R + kt.R;
    float sumG = kd.G + ks.G + kt.G;
    float sumB = kd.B + ks.B + kt.B;
    if (sumR > 1.0f) {
        float f = 0.999f / sumR;
        kd.R *= f; ks.R *= f; kt.R *= f;
    }
    if (sumG > 1.0f) {
        float f = 0.999f / sumG;
        kd.G *= f; ks.G *= f; kt.G *= f;
    }
    if (sumB > 1.0f) {
        float f = 0.999f / sumB;
        kd.B *= f; ks.B *= f; kt.B *= f;
    }
}

// Fábricas para materiales predeterminados (se usan para inicialización global)
static Material makeWhiteDiffuse() {
    Color kd = white; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makeRedDiffuse() {
    Color kd = red; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makeGreenDiffuse() {
    Color kd = green; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makeBlueDiffuse() {
    Color kd = blue; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makePurpleDiffuse() {
    Color kd = purple; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makeCyanDiffuse() {
    Color kd = cyan; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}
static Material makeGrayDiffuse() {
    Color kd = gray; Color ks = black; Color kt = black; Color Le = black; float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}

static Material makeMirrorSpecular() {
    Color kd = black;
    Color ks(1.0f, 1.0f, 1.0f);
    Color kt = black;
    Color Le = black;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, 1.0f);
}

static Material makeGlassDielectric() {
    Color kd(0.0f, 0.0f, 0.0f);       // sin difusión
    Color ks(0.04f, 0.04f, 0.04f);    // reflexión mínima
    Color kt(0.96f, 0.96f, 0.96f);    // muy transparente
    float ior = 1.6f;                 // vidrio/plomo para cáusticas marcadas
    Color Le(0.0f, 0.0f, 0.0f);       // sin emisión
    return Material(kd, ks, kt, Le, ior);
}

static Material makePlasticMaterial() {
    // Plástico básico: difuso azul + pequeño brillo especular (no delta)
    // Ligeramente más claro y con brillo un poco mayor (pero <0.1 para no activar "isSpecular")
    Color kd(0.35f, 0.55f, 0.95f); // azul claro tipo botella de agua
    Color ks(0.2f, 0.2f, 0.2f); // brillo especular más visible pero no espejo
    Color kt = black;
    Color Le = black;
    float ior = 1.0f;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, ior);
}

// Materiales llamativos para pruebas
static Material makeNeonEmissive() {
    // Emissive neon (muy brillante, para pruebas rápidas)
    Color kd = black;
    Color ks = black;
    Color kt = black;
    Color Le(8.0f, 0.2f, 8.0f); // magenta/neón potente
    float ior = 1.0f;
    return Material(kd, ks, kt, Le, ior);
}

static Material makeShinyGold() {
    // Metal dorado: baja difusa, alta especular en tonos dorados
    Color kd(0.15f, 0.065f, 0.01f);
    Color ks(0.9f, 0.76f, 0.3f); // color especular metálico
    Color kt = black;
    Color Le = black;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, 1.0f);
}

static Material makeGlossyPlasticStrong() {
    // Plástico brillante: mayor componente especular no delta (aunque el
    // render actual trata ks>0.1 como delta, esta configuración dará un
    // aspecto más brillante cuando se ajuste el muestreo specular).
    Color kd(0.2f, 0.4f, 0.85f); // azul intenso
    Color ks(0.35f, 0.35f, 0.35f); // especular más fuerte
    Color kt = black;
    Color Le = black;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, 1.0f);
}

static Material makeColoredGlassStrong() {
    // Vidrio coloreado: transmisión alta y ligera reflectancia
    Color kd = black;
    Color ks(0.02f, 0.02f, 0.02f);
    Color kt(0.85f, 0.4f, 0.95f); // rosa / magenta translúcido
    Color Le = black;
    normalizeColors(kd, ks, kt);
    return Material(kd, ks, kt, Le, 1.45f);
}

// Inicialización de los materiales globales
Material whiteDiffuse  = makeWhiteDiffuse();
Material redDiffuse    = makeRedDiffuse();
Material greenDiffuse  = makeGreenDiffuse();
Material blueDiffuse   = makeBlueDiffuse();
Material purpleDiffuse = makePurpleDiffuse();
Material cyanDiffuse   = makeCyanDiffuse();
Material grayDiffuse   = makeGrayDiffuse();
Material mirrorSpecular = makeMirrorSpecular();
Material glassDielectric = makeGlassDielectric();
Material plasticMaterial = makePlasticMaterial();
Material shinyGold = makeShinyGold();
Material glossyPlasticStrong = makeGlossyPlasticStrong();
Material coloredGlassStrong = makeColoredGlassStrong();


//Para las luces neon emisivo
Material neonEmissive = makeNeonEmissive();


