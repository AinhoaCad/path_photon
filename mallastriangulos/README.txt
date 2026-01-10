Autores : 

Ainhoa Cadena Sola 

Pedro Benito Serrano


COMPILACIÓN
---------------------------------------------

Primero compilar el archivo Makefile, situado en la raíz del proyecto,
para generar el ejecutable y los .obj : 

-------------------------------------
make clean 
make 
-------------------------------------

Una vez se genere el ejecutable, se encontrará en el subdirectorio bin del proyecto. El ejecutable se llama : 

-MallaTriangulos : pathtracer

Comando de ejecución : .\bin\pathtracer <spp> <dim> <dim> <fichero_salida>  (el formato puede ser .ppm, .exr, .png o .pfm)
Parámetros: <spp> samples per pixel, <dim> <dim> anchura y altura en píxeles.

*La escena a renderizar se elige desde el main de entre las diponibles.

Desde el main se pueden ajustar diferentes parámetros : 

-  useSmoothNormal : dice si se quieren las imágenes con normales suavizadas o no
-  trianglePercentage : porcentaje de la malla de triangulos que se desea cargar

NOTA IMPORTANTE SOBRE BVH Y EMBREE
----------------------------------------------------------

La parte correspondiente al uso de BVH para acelerar las intersecciones
rayo–malla utiliza la biblioteca Embree.

Durante el desarrollo, Embree se ha utilizado desde un entorno Linux
ejecutado mediante WSL (Windows Subsystem for Linux).

CONFIGURACIÓN SEGÚN EL SISTEMA
----------------------------------------------------------

• Windows (sin WSL):
  - El proyecto se compila y ejecuta sin Embree.
  - El path tracer funciona correctamente con mallas de triángulos,
    pero el BVH basado en Embree queda desactivado automáticamente.
  - No es necesario instalar dependencias adicionales.

• Linux / WSL:
  - Si se compila el proyecto desde WSL con Embree instalado,
    el BVH se activa automáticamente.
  - Para instalar Embree en WSL:

    sudo apt update
    sudo apt install libembree-dev
    dpkg -l | grep embree

DETECCIÓN AUTOMÁTICA EN EL MAKEFILE
----------------------------------------------------------

El Makefile detecta automáticamente el sistema operativo:

- Si detecta Windows, compila el proyecto sin soporte para Embree.
- Si detecta Linux / WSL, compila con soporte para Embree activado
  mediante la macro USE_EMBREE y enlaza contra la librería correspondiente.

Si el sistema no es Linux o Embree no está disponible, las funcionalidades
de BVH quedan desactivadas automáticamente, manteniendo el funcionamiento
correcto del path tracer base.

De este modo, el proyecto puede compilarse y ejecutarse en cualquier
entorno sin requerir obligatoriamente Embree, utilizando BVH únicamente
cuando está disponible.

NOTAS
------------------------------------
Para la lectura y escritura de imágenes en distintos formatos se han utilizado
librerías third-party integradas directamente en el proyecto.

En concreto:

- Para el formato PNG se utiliza la librería stb_image para la lectura y una
  implementación basada en miniz para la escritura en memoria.

- Para el formato EXR (High Dynamic Range) se emplea la librería TinyEXR,
  que permite la lectura y escritura de imágenes HDR en coma flotante.

- Los formatos PPM y PFM han sido implementados manualmente dentro de la
  clase Image, siguiendo las especificaciones estándar de dichos formatos.

Todas las librerías third-party se encuentran incluidas en la carpeta
third_party del proyecto, los cpp correspondientes están integrados en el src y debidamente documentados,
por lo que no es necesario instalar dependencias externas adicionales para compilar y ejecutar el programa.
