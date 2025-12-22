# Guía de Tone Mapping y Formatos de Imagen HDR/LDR

## Resumen de Cambios

Se ha ampliado la clase `Image` para soportar:

### 1. Múltiples Formatos de Imagen
- **PPM** (.ppm) - Low Dynamic Range (ASCII)
- **PFM** (.pfm) - High Dynamic Range (ASCII)
- **EXR** (.exr) - High Dynamic Range (Binario, requiere OpenEXR)
- **PNG** (.png) - Low Dynamic Range (Binario, requiere libpng)

### 2. Operadores de Tone Mapping

#### Clamping
```cpp
image.clampPixels(1.0f);  // Acotar valores a [0, 1]
```
Fórmula: `L_out = min(L_in, 1)`

#### Ecualización (Normalización)
```cpp
image.ecualizacion();  // Normalizar desde min a max
```
Fórmula: `L_out = (L_in - L_min) / (L_max - L_min)`

#### Ecualización + Clamp
```cpp
image.ecualizacionClamp(0.5f);  // Parámetro V = 0.5
```
Fórmula: `L_out = min((L_in - L_min) / (L_max - L_min), V)`

#### Curva Gamma
```cpp
image.curvaGamma(2.2f);  // Aplicar gamma = 2.2
```
Fórmula: `L_out = ((L_in - L_min) / (L_max - L_min))^gamma`

#### Gamma + Clamp
```cpp
image.gammaClamp(2.2f);  // Clamp primero, luego gamma
```
Fórmula: `L_out = (min(L_in, 1) / L_max)^gamma`

### 3. Método Genérico de Tone Mapping

```cpp
enum class ToneMappingOperator {
    CLAMPING,           // Simple clamping
    EQUALIZATION,       // Linear normalization
    EQUALIZATION_CLAMP, // Equalization + parameter clamping
    GAMMA,              // Gamma curve (includes equalization)
    GAMMA_CLAMP         // Gamma curve after clamping
};

// Uso
image.applyToneMapping(ToneMappingOperator::GAMMA, 2.2f);
```

### 4. Métodos de Lectura/Escritura Genéricos

```cpp
// Detecta formato automáticamente por extensión
image.write("salida.ppm");   // Escribe en PPM
image.write("salida.pfm");   // Escribe en PFM (HDR)
image.write("salida.png");   // Escribe en PNG

bool success = image.read("entrada.exr");  // Lee desde EXR
```

### 5. Utilidades de Luminancia

```cpp
// Obtener información sobre luminancia
float maxLum = image.getMaxLuminance();
float minLum = image.getMinLuminance();
float lum = image.getLuminance(pixel);  // Usa ITU-R BT.709
```

## Ejemplo de Uso Completo

```cpp
#include "Image.hpp"

int main() {
    // Crear imagen HDR renderizada
    Image hdrImage(1920, 1080);
    // ... llenar con datos de renderizado ...
    
    // Guardar imagen HDR sin procesar (formato flotante)
    hdrImage.write("salida_hdr.pfm");      // O .exr si OpenEXR está disponible
    
    // Crear copia para tone mapping
    Image ldrImage = hdrImage;  // Necesitarías agregar copy constructor
    
    // Aplicar operadores de tone mapping
    ldrImage.applyToneMapping(ToneMappingOperator::EQUALIZATION);
    ldrImage.write("salida_equalization.ppm");
    
    // Con gamma
    ldrImage = hdrImage;
    ldrImage.applyToneMapping(ToneMappingOperator::GAMMA, 2.2f);
    ldrImage.write("salida_gamma.ppm");
    
    // Con ecualización + clamp
    ldrImage = hdrImage;
    ldrImage.applyToneMapping(ToneMappingOperator::EQUALIZATION_CLAMP, 0.8f);
    ldrImage.write("salida_eq_clamp.ppm");
    
    return 0;
}
```

## Compilación

### Sin OpenEXR (Compatible con Windows por defecto)
```bash
make
```

### Con OpenEXR (Linux/WSL)
Instalar librerías:
```bash
sudo apt-get install libopenexr-dev
```

Compilar con flag:
```bash
g++ -DUSE_OPENEXR -o programa main.cpp src/*.cpp -lopenexr
```

## Notas Importantes

1. **PFM vs EXR**: PFM es más sencillo pero requiere menos dependencias. EXR es más estándar pero requiere OpenEXR.

2. **Gamma Correction**: El valor estándar es `gamma = 2.2` para sRGB.

3. **Luminancia**: Se usa el estándar ITU-R BT.709 para calcular luminancia:
   - Fórmula: `L = 0.2126*R + 0.7152*G + 0.0722*B`

4. **Rango Dinámico**: 
   - HDR: Valores float arbitrarios (típicamente 0 a infinito)
   - LDR: Valores en rango [0, 1] o [0, 255] (enteros)

## Próximos Pasos Sugeridos

1. Agregar copy constructor y operator= a la clase Image
2. Implementar más operadores de tone mapping avanzados
3. Agregar soporte para compresión en formatos binarios
4. Implementar lectura/escritura de metadatos (EXIF, etc.)
