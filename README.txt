Autores : 

Ainhoa Cadena Sola 

Pedro Benito Serrano

DISTRIBUCIÓN DE SECCIONES
-------------------------------------------
El proyecto se divide en tres directorios, cada uno encargado de una sección

-PathTracer_Junto : Sección correspondiente a nuestro PathTracer
-PhotonMapping: Sección correspondiente a nuestro PhotonMapping
-extension: Sección correspondiente a la extensión de malla de triángulos sobre nuestro PathTracer

COMPILACIÓN
---------------------------------------------

Primero navegar a la sección sobre la que se desea realizar el render, para ejecutar el fichero Makefile correspondiente

-------------------------------------
make clean 
make 
-------------------------------------

Una vez se genere el ejecutable, se encontrará en el subdirectorio bin de cada proyecto. Cada ejecutable se llama : 

-PathTracer_Junto : pathtracer

Comando de ejecución : .\bin\pathtracer <spp> <dim> <dim> <fichero_salida>  (el formato puede ser .ppm, .exr, .png o .pfm)
Parámetros: <spp> samples per pixel, <dim> <dim> anchura y altura en píxeles.

-PhotonMapping : photonmapping

Comando de ejecución : .\bin\photonmapping <spp> <dim> <dim> <fichero_salida>  (el formato puede ser .ppm, .exr, .png o .pfm)
Parámetros: <spp> samples per pixel, <dim> <dim> anchura y altura en píxeles.

-extension : pathtracer

Comando de ejecución : .\bin\pathtracer <spp> <dim> <dim> <fichero_salida>  (el formato puede ser .ppm, .exr, .png o .pfm)
Parámetros: <spp> samples per pixel, <dim> <dim> anchura y altura en píxeles.
(PARA EMBREE ES NECESARIO EJECUTAR DESDE WSL)