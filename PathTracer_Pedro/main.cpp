#include "include/Scene.hpp"
#include "include/Renderizador.hpp"
#include "include/Camera.hpp"
#include "include/Sphere.hpp"
#include "include/Plane.hpp"
#include "include/Light.hpp"

#include <cassert>
#include <iostream>


Scene createCornellBox() {
    Scene scene;

    // Materiales
    Material redDiffuse(Color(1.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material greenDiffuse(Color(0.0f, 1.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);
    Material whiteDiffuse(Color(0.8f, 0.8f, 0.8f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), 1.0f);

    // Paredes como planos
    scene.addPlane(Plane(Vector3(-1.0f, 0.0f, 0.0f), 1.0f, redDiffuse));   // Izquierda
    scene.addPlane(Plane(Vector3(1.0f, 0.0f, 0.0f), -1.0f, greenDiffuse)); // Derecha
    scene.addPlane(Plane(Vector3(0.0f, -1.0f, 0.0f), 1.0f, whiteDiffuse)); // Piso
    scene.addPlane(Plane(Vector3(0.0f, 1.0f, 0.0f), -1.0f, whiteDiffuse)); // Techo
    scene.addPlane(Plane(Vector3(0.0f, 0.0f, 1.0f), 3.0f, whiteDiffuse));  // Fondo

    // Esferas
    scene.addSphere(Sphere(Vector3(-0.5f, -0.7f, 0.25), 0.3f, whiteDiffuse));
    scene.addSphere(Sphere(Vector3(0.5f, -0.7f, -0.25), 0.3f, whiteDiffuse));
    
    // Luz puntual
    scene.addLight(PointLight(Vector3(0.0f, 0.9f, -2.0f), Color(10.0f, 10.0f, 10.0f)));

    std::cout << "Escena creada." << std::endl;

    return scene;
}



void testCameraConfiguration() {
    Camera camera;
    camera.origen = Vector3(0.0f, 0.0f, 0.0f);
    camera.l = Vector3(-1.0f, 0.0f, 0.0f);
    camera.u = Vector3(0.0f, 1.0f, 0.0f);
    camera.f = Vector3(0.0f, 0.0f, -1.0f);
    camera.numPixelsAncho = 256;
    camera.numPixelsAlto = 256;
    camera.nMuestras = 30;
    camera.Initialize(); // Configuración de píxeles

    assert(camera.origen == Vector3(0.0f, 0.0f, 0.0f));
    assert(camera.l == Vector3(-1.0f, 0.0f, 0.0f));
    assert(camera.u == Vector3(0.0f, 1.0f, 0.0f));
    assert(camera.f == Vector3(0.0f, 0.0f, -1.0f));
    assert(camera.numPixelsAncho == 256);
    assert(camera.numPixelsAlto == 256);
    assert(camera.nMuestras == 30);

    std::cout << "Configuración de la cámara verificada correctamente." << std::endl;
}

void testCreateCornellBox() {
    Scene scene = createCornellBox();

    assert(scene.getPlanes().size() == 5);
    assert(scene.getSpheres().size() == 2);
    assert(scene.getLights().size() == 1);

    std::cout << "Creación de la escena verificada correctamente." << std::endl;
}

void testRenderizadorConfiguration() {
    Scene scene = createCornellBox();
    Renderizador renderizador(scene);

    assert(renderizador.getScene().getPlanes().size() == 5);
    assert(renderizador.getScene().getSpheres().size() == 2);
    assert(renderizador.getScene().getLights().size() == 1);

    std::cout << "Configuración del renderizador verificada correctamente." << std::endl;
}

void testRender() {
    Camera camera;
    camera.origen = Vector3(0.0f, 0.0f, 0.0f);
    camera.l = Vector3(-1.0f, 0.0f, 0.0f);
    camera.u = Vector3(0.0f, 1.0f, 0.0f);
    camera.f = Vector3(0.0f, 0.0f, -1.0f);
    camera.numPixelsAncho = 256;
    camera.numPixelsAlto = 256;
    camera.nMuestras = 30;
    camera.Initialize(); // Configuración de píxeles

    Scene scene = createCornellBox();
    Renderizador renderizador(scene);

    std::ofstream output("test_output.ppm");
    if (!output.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura." << std::endl;
        return;
    }

    camera.render("test_output.ppm", renderizador, 5);
    output.close();

    std::ifstream input("test_output.ppm");
    assert(input.is_open());

    std::cout << "Renderización verificada correctamente." << std::endl;
}



int main() {
    //Ejecución de pruebas
    testCameraConfiguration();
    testCreateCornellBox();
    testRenderizadorConfiguration();
    testRender();

    // Configuración de la cámara
    Camera camera;
    camera.origen = Vector3(0.0f, 0.0f, 0.0f);
    camera.l = Vector3(-1.0f, 0.0f, 0.0f);
    camera.u = Vector3(0.0f, 1.0f, 0.0f);
    camera.f = Vector3(0.0f, 0.0f, -1.0f);
    camera.numPixelsAncho = 256;
    camera.numPixelsAlto = 256;
    camera.nMuestras = 30;
    camera.Initialize(); // Configuración de píxeles

    std::cout << "Configuracion de la camara : " << std::endl;
    std::cout << "Origen : " << camera.origen << std::endl;
    std::cout << "L : " << camera.l << std::endl;
    std::cout << "U : " << camera.u << std::endl;
    std::cout << "F : " << camera.f << std::endl;
    std::cout << "Numero de pixeles de ancho : " << camera.numPixelsAncho << std::endl;
    std::cout << "Numero de pixeles de alto : " << camera.numPixelsAlto << std::endl;
    std::cout << "Numero de muestras: " << camera.nMuestras << std::endl;    

    // Creación de la escena
    Scene scene = createCornellBox();

    // Configuración del renderizador
    Renderizador renderizador(scene);

    // Renderización
    std::ofstream output("output.ppm");
    if (!output.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para escritura." << std::endl;
        return 1;
    }

    // Renderizar escena con profundidad máxima de 5
    renderizador.calcularColorRayo(camera.crearRayo(400, 300), 5);

    // Generar la imagen final
    camera.render("output.ppm", renderizador, 5);

    output.close();
    std::cout << "Renderizado completado. Imagen guardada como 'output.ppm'." << std::endl;

    return 0;
}
