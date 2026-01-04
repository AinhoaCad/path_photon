# Formato de Salida Flexible - Guía Rápida

## Cómo Usar

Simplemente **cambia la extensión del archivo de salida** y el programa detectará automáticamente el formato:

### Ejemplos

```bash
# Guardar como PPM (Low Dynamic Range)
./pathtracer 1920 1080 100 salida.ppm

# Guardar como PFM (High Dynamic Range - requiere VERIV para visualizar)
./pathtracer 1920 1080 100 salida.pfm

# Guardar como EXR (High Dynamic Range - si OpenEXR está instalado)
./pathtracer 1920 1080 100 salida.exr

# Guardar como PNG (si libpng está disponible)
./pathtracer 1920 1080 100 salida.png
```

## Formatos Disponibles

| Formato | Extensión | Tipo | Requisitos | Visualización |
|---------|-----------|------|-----------|---------------|
| PPM | `.ppm` | LDR | Ninguno | Visor estándar |
| PFM | `.pfm` | HDR | Ninguno | VERIV (VS Code) |
| EXR | `.exr` | HDR | OpenEXR (opcional) | VERIV (VS Code), Tev |
| PNG | `.png` | LDR | libpng (opcional) | Visor estándar |

## Instalación de VERIV en VS Code

Para visualizar imágenes HDR (`.exr` y `.pfm`) con VERIV:

1. Abre VS Code
2. Ve a Extensiones (Ctrl+Shift+X)
3. Busca "VERIV"
4. Instala la extensión de mcrescas

**URL:** https://github.com/mcrescas/veriv

Una vez instalado, solo abre cualquier archivo `.exr` o `.pfm` y se visualizará directamente en VS Code.

## Notas Importantes

- **PPM/PNG**: Valores enteros 0-255 (LDR - bajo rango dinámico)
- **PFM/EXR**: Valores flotantes (HDR - alto rango dinámico, mantiene precisión)
- Los archivos se guardan en la carpeta raíz del proyecto
- Los archivos con sufijo `_BVH` y `_Embree` se generan automáticamente

## Sin Cambios en el Código

No necesitas modificar nada en el código C++ - simplemente cambia la extensión del archivo cuando ejecutes el programa. ¡Eso es todo!
