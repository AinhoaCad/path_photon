--------------------------------------------------------------------------------
Compilación del proyecto:
    - Se dispone de un fichero Makefile en la raíz que compila
        y genera un binario 'photonmapping' que permite mediante
        parámetros escoger escena, resolución, y más opciones
--------------------------------------------------------------------------------
Ejecución:
 ./photonmapping -s <escena> [-r <numRayos>] [-p numPaseos] [-R radio] 
    [-k kVecinos] [-a <apertura>] [-t <numThreads>] [-N <altura>] 
    [-M <anchura>] [-c <resolucionColor>] [-K kernel]

    -s se especifica mediante un número siguiendo la codificación explicada
    -r especifica con un entero el número de rayos por píxel (por defecto 5)
    -p es el número de fotones emitidos desde las luces formando caminos 
        (por defecto 1000)
    -R especifica el radio inicial en la estimación de radiancia 
        (por defecto 0.25)
    -a es la apertura de la lente como valor real entre 0 y 1 (por defecto 0)
    -t es el número de hilos de ejecucion para la paralelización (por defecto 1)
    -N es la altura de la imagen en píxeles (por defecto 256)
    -M es la anchura de la imagen en píxeles (por defecto 256)
    -c es la resolucion de color en la imagen resultante (por defecto 256)
    
--------------------------------------------------------------------------------
Escenas:
    Luces puntuales
    ---------------
    0. Cornell Box difusa con dos esferas y luz puntual
    1. Cornell Box con esferas reflexiva y dieléctrica con una luz puntual
    2. Cornell Box con esfera difusa y dos luces puntuales
    3. Cornell Box con tres esferas difusas y una luz puntual
    4. Cornell Box con texturas
    5. Cornell Box con esfera especular y luz puntual

    Escenas elaboradas
    ------------------
    6. Escena con conos, cilindros, caja, disco, esfera y planos
    7. Escena con un árbol cónico de luz puntual
    8. Escena ajedrez
--------------------------------------------------------------------------------
Autores:
    - Santiago Illa Criado  805798
    - Carlos Paesa Lía      798974
--------------------------------------------------------------------------------
