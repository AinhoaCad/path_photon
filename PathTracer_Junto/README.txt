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

-PathTracer : pathtracer

Comando de ejecución : .\bin\pathtracer <spp> <dim> <dim> <fichero_salida>  (el formato puede ser .ppm, .exr, .png o .pfm)
Parámetros: <spp> samples per pixel, <dim> <dim> anchura y altura en píxeles.


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
