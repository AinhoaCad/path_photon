#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "image.hpp"
#include "triangle.hpp"
#include "fileParser.hpp"
#include "photonMapping.hpp"
#include <iostream>
#include <vector>
#include <chrono>  // Librería para medir el tiempo
#include <triangleMesh.hpp>
#include "cylinder.hpp"

using namespace std;
using namespace std::chrono;

void CornellBox(){
    Camera camera(Point(0,0,-3.5),Direction(-1,0,0),Direction(0,1,0),Direction(0,0,3));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB((float)218/255,(float)176/255,(float)218/255),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB((float)218/510,(float)176/510,(float)218/510),RGB((float)218/510,(float)218/510,(float)218/510),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0.4,0.3,0.4),RGB(0.4,0.3,0.4),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0,1,1),RGB(0,1,1),RGB(0,0,0)));
    Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0.0,0.0,0.0),RGB(0.99,0.99,0.99),RGB(0,0,0),1.5));
    Primitive* planeLeft = new Plane(Direction(1,0,0),1.0,Material(RGB(0.7,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeRight = new Plane(Direction(-1,0,0),1.0,Material(RGB(0.0,0.7,0.0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeFloor = new Plane(Direction(0,1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(1.0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* triangle = new Triangle(Point(-0.5,-0.7,0.25),Point(0.5,0,0),Point(0.5,-0.7,-0.25),Material(RGB((float)255/255,(float)0/255,(float)255/255),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    vector<Primitive*> primitives;
    primitives.push_back(planeBack);
    primitives.push_back(planeLeft);
    primitives.push_back(planeRight);
    primitives.push_back(planeFloor);
    primitives.push_back(planeCeiling);
    //primitives.push_back(triangle);
    primitives.push_back(sphereLeft);
    primitives.push_back(sphereRight);
    LightSource* c1= new PointLight(Point(0,0.5,0),RGB(1,1,1));
    LightSource* c2= new AreaLight(*planeCeiling);
    vector<LightSource*> lightSources;
    lightSources.push_back(c1);
    //lightSources.push_back(c2);
    Scene scene(camera,primitives,lightSources);
    
    Image image(256,256);
    int nwalks=10;
    auto start = high_resolution_clock::now();
    PhotonMapping pm(nwalks,image);
    pm.parallelization(scene);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Renderizado completado en " << duration.count() << " microsegundos." << endl;
    Image imagen2 = pm.getImage();
    imagen2.applyToneMappingOperator("clamp_gamma",imagen2.max());
    FileParser::writefilePPM(imagen2,"salida_photonMapping_4.ppm");
    cout << "acaba"<<endl;
}

void createCornellBoxTriangle(){
    Camera camera(Point(0,0,-3.5),Direction(-1,0,0),Direction(0,1,0),Direction(0,0,3));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB((float)218/255,(float)176/255,(float)218/255),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB((float)218/510,(float)176/510,(float)218/510),RGB((float)218/510,(float)218/510,(float)218/510),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0.4,0.3,0.4),RGB(0.4,0.3,0.4),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0,1,1),RGB(0,1,1),RGB(0,0,0)));
    Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0.0,0.0,0.0),RGB(0.99,0.99,0.99),RGB(0,0,0),1.5));
    Primitive* planeLeft = new Plane(Direction(1,0,0),1.0,Material(RGB(0.7,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeRight = new Plane(Direction(-1,0,0),1.0,Material(RGB(0.0,0.7,0.0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeFloor = new Plane(Direction(0,1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(1,1,1),1));
    Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(1.0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* triangle = new Triangle(Point(-0.5,-0.3,0.25),Point(0.35,0.2,0.2),Point(0.45,-0.5,0.2),Material(RGB(0.9,0.0,0.9),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    vector<Primitive*> primitives;
    primitives.push_back(planeBack);
    primitives.push_back(planeLeft);
    primitives.push_back(planeRight);
    primitives.push_back(planeFloor);
    primitives.push_back(planeCeiling);
    primitives.push_back(triangle);
    //primitives.push_back(sphereLeft);
    //primitives.push_back(sphereRight);
    LightSource* c1= new PointLight(Point(0,0.5,0),RGB(5,5,5));
    LightSource* c2= new AreaLight(*planeCeiling);
    vector<LightSource*> lightSources;
    lightSources.push_back(c1);
    //lightSources.push_back(c2);
    Scene scene(camera,primitives,lightSources);
    
    Image image(256,256);
    int nwalks=10;
    auto start = high_resolution_clock::now();
    PhotonMapping pm(nwalks,image);
    pm.parallelization(scene);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Renderizado completado en " << duration.count() << " microsegundos." << endl;
    Image imagen2 = pm.getImage();
    imagen2.applyToneMappingOperator("clamp_gamma",imagen2.max());
    FileParser::writefilePPM(imagen2,"triangulo.ppm");
    cout << "acaba"<<endl;
}

void CornellBoxCilindro(){
    Camera camera(Point(0,0,-3.5),Direction(-1,0,0),Direction(0,1,0),Direction(0,0,3));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB((float)218/255,(float)176/255,(float)218/255),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB((float)218/510,(float)176/510,(float)218/510),RGB((float)218/510,(float)218/510,(float)218/510),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0.4,0.3,0.4),RGB(0.4,0.3,0.4),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0,1,1),RGB(0,1,1),RGB(0,0,0)));
    Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0.0,0.0,0.0),RGB(0.99,0.99,0.99),RGB(0,0,0),1.5));
    Primitive* planeLeft = new Plane(Direction(1,0,0),1.0,Material(RGB(0.7,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeRight = new Plane(Direction(-1,0,0),1.0,Material(RGB(0.0,0.7,0.0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeFloor = new Plane(Direction(0,1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(1,1,1),1));
    Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(0.5,0.5,0.5),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(1.0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* triangle = new Triangle(Point(-0.5,-0.7,0.25),Point(0.5,0,0),Point(0.5,-0.7,-0.25),Material(RGB((float)255/255,(float)0/255,(float)255/255),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* cylinder = new Cylinder(Direction(0,1,0),Point(-0.5,-0.7,0.25),0.7,0.3,Material(RGB(0.5,0.4,0.3),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    vector<Primitive*> primitives;
    primitives.push_back(planeBack);
    primitives.push_back(planeLeft);
    primitives.push_back(planeRight);
    primitives.push_back(planeFloor);
    primitives.push_back(planeCeiling);
    //primitives.push_back(triangle);
    //primitives.push_back(sphereLeft);
    //primitives.push_back(sphereRight);
    primitives.push_back(cylinder);
    LightSource* c1= new PointLight(Point(0,0.75,0),RGB(5,5,5));
    LightSource* c2= new AreaLight(*planeCeiling);
    vector<LightSource*> lightSources;
    lightSources.push_back(c1);
    //lightSources.push_back(c2);
    Scene scene(camera,primitives,lightSources);
    
    Image image(256,256);
    int nwalks=10;
    auto start = high_resolution_clock::now();
    PhotonMapping pm(nwalks,image);
    pm.parallelization(scene);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Renderizado completado en " << duration.count() << " microsegundos." << endl;
    Image imagen2 = pm.getImage();
    imagen2.applyToneMappingOperator("clamp_gamma",imagen2.max());
    FileParser::writefilePPM(imagen2,"cilindro.ppm");
    cout << "acaba"<<endl;
}


void triangleMeshScene(){
    Camera camera(Point(0,0.5,-3.5),Direction(-1,0,0),Direction(0,1,0),Direction(0,0,3));
    Primitive* planeLeft = new Plane(Direction(1,0,0),1.0,Material(RGB(0.2,0.2,0.2),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeRight = new Plane(Direction(-1,0,0),1.0,Material(RGB(0.2,0.2,0.2),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeFloor = new Plane(Direction(0,1,0),-0.1,Material(RGB(0.0,0.0,0.9),RGB(0.1,0.1,0.1),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.5,Material(RGB(0.2,0.2,0.8),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(0.2,0.2,0.2),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeGlass = new Plane(Direction(0,0,-1),0.5,Material(RGB(0,0,0),RGB(0.0,0.0,0.0),RGB(0.09,0.09,0.99),RGB(0,0,0),1.5));
    vector<Primitive*> primitives;
    primitives.push_back(planeBack);
    primitives.push_back(planeLeft);
    primitives.push_back(planeRight);
    primitives.push_back(planeFloor);
    //primitives.push_back(planeCeiling);
    //primitives.push_back(planeGlass);
    LightSource* c1= new PointLight(Point(0,1.2,0),RGB(1,1,1));
    LightSource* c2= new AreaLight(*planeCeiling);
    vector<LightSource*> lightSources;
    lightSources.push_back(c1);
    //lightSources.push_back(c2);
    TriangleMesh mesh=FileParser::readObj("boat2.obj");
    primitives.push_back(&mesh);
    Scene scene(camera,primitives,lightSources);
    
    Image image(256,256);
    int nwalks=10;
    auto start = high_resolution_clock::now();
    PhotonMapping pm(nwalks,image);
    pm.parallelization(scene);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);
    cout << "Renderizado completado en " << duration.count() << " segundos." << endl;
    Image imagen2 = pm.getImage();
    imagen2.applyToneMappingOperator("clamp_gamma",imagen2.max());
    FileParser::writefilePPM(imagen2,"barco.ppm");
    cout << "acaba"<<endl;
}

void triangleMeshScene2(){
    Camera camera(Point(0,0,-3.5),Direction(-1,0,0),Direction(0,1,0),Direction(0,0,3));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB((float)218/255,(float)176/255,(float)218/255),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB((float)218/510,(float)176/510,(float)218/510),RGB((float)218/510,(float)218/510,(float)218/510),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* sphereLeft = new  Sphere(Point(-0.5,-0.7,0.25),Direction(0.3,0,0),Material(RGB(0.4,0.3,0.4),RGB(0.4,0.3,0.4),RGB(0,0,0),RGB(0,0,0),1));
    //Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0,1,1),RGB(0,1,1),RGB(0,0,0)));
    Primitive* sphereRight = new  Sphere(Point(0.5,-0.7,-0.25),Direction(0.3,0,0),Material(RGB(0,0,0),RGB(0.0,0.0,0.0),RGB(0.99,0.99,0.99),RGB(0,0,0),1.5));
    Primitive* planeLeft = new Plane(Direction(1,0,0),1.0,Material(RGB(0.1,0.0,0.45),RGB(0.1,0.0,0.45),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeRight = new Plane(Direction(-1,0,0),1.0,Material(RGB(0.1,0.0,0.45),RGB(0.1,0.0,0.45),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeFloor = new Plane(Direction(0,1,0),1.0,Material(RGB(0.0,0.5,0.1),RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),1));
    Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0.1,0.0,0.45),RGB(0.1,0.0,0.45),RGB(0,0,0),RGB(0,0,0),1.5));
    //Primitive* planeCeiling= new Plane(Direction(0,-1,0),1.0,Material(RGB(0,0,0),RGB(0,0,0),RGB(0,0,0),RGB(1,1,1),1));
    Primitive* planeBack = new Plane(Direction(0,0,-1),1.0,Material(RGB(0.1,0.0,0.45),RGB(0.1,0.0,0.45),RGB(0,0,0),RGB(0,0,0),1));
    vector<Primitive*> primitives;
    primitives.push_back(planeBack);
    primitives.push_back(planeLeft);
    primitives.push_back(planeRight);
    primitives.push_back(planeFloor);
    primitives.push_back(planeCeiling);
    //primitives.push_back(planeGlass);
    LightSource* c1= new PointLight(Point(0,0.5,0),RGB(1,1,1));
    LightSource* c2= new AreaLight(*planeCeiling);
    vector<LightSource*> lightSources;
    lightSources.push_back(c1);
    //lightSources.push_back(c2);
    TriangleMesh mesh=FileParser::readObj("fox2.obj");
    primitives.push_back(&mesh);
    Scene scene(camera,primitives,lightSources);
    
    Image image(256,256);
    int nwalks=10;
    auto start = high_resolution_clock::now();
    PhotonMapping pm(nwalks,image);
    pm.parallelization(scene);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);
    cout << "Renderizado completado en " << duration.count() << " segundos." << endl;
    Image imagen2 = pm.getImage();
    imagen2.applyToneMappingOperator("clamp_gamma",imagen2.max());
    FileParser::writefilePPM(imagen2,"fox.ppm");
    cout << "acaba"<<endl;
}

int main(int argc, char const *argv[]){
    //CornellBox();
    //triangleMeshScene();
    //triangleMeshScene2();
    createCornellBoxTriangle();
    //CornellBoxCilindro();
    return 0;
}
