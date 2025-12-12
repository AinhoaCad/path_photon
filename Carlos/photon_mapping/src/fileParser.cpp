#include <iostream>
#include <fstream>  // Para ifstream
#include <ostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include "image.hpp"
#include "fileParser.hpp"
#include <triangleMesh.hpp>
#include "material.hpp"


using namespace std;



//*********************************************    PPM    ************************************************************** */
Image FileParser::readfilePPM(string ficheroEntrada) {
    ifstream fich(ficheroEntrada);
    if (!fich.is_open()) {
        cout << "Error al abrir el fichero de entrada\n";
        exit(EXIT_FAILURE);
    }

    string line;
    string format;
    int w=0;
    int h=0;

    // Leer formato (P3)
    getline(fich, format);

    float m;
    
    // Leer posibles comentarios o línea vacía
    while (getline(fich, line)) {
        if (line.empty() || line[0] == '#') {
            if (line.find("MAX=") != string::npos) {
                // Extraer el valor de MAX
                string max_str = line.substr(line.find('=') + 1);
                m = stof(max_str);  // Convertir a entero
            }
            continue; // Ignorar comentarios
        } else {
            istringstream iss(line);
            iss >> w >> h;
            break;
        }
    }
    Image imagen= Image(w,h);

    // Leer la resolución máxima del color
    float c;
    fich >> c;

    //leer RGBes
    for (int i = 0; i < imagen.width*imagen.height; ++i){
        RGB p;
        fich >> p.r >> p.g >> p.b;
        p.r=p.r*m/c;
        p.g= p.g*m/c;
        p.b= p.b*m/c;
        imagen.RGBes[i] = p;
    }
    return imagen;
    fich.close();
    return imagen;
}



void FileParser::writefilePPM(Image imagen,string fichSalida){
    int c=255;
    ofstream salida(fichSalida);
    if (!salida.is_open()) {
        exit(EXIT_FAILURE);
    }
    float newM=imagen.max();
    salida<<"P3"<<endl;
    salida<<"#MAX="<<fixed<<newM<<endl;
    salida<<imagen.width<<" "<<imagen.height<<endl;
    salida<<std::setprecision(0)<<fixed<<c<<endl;
    

    for (int i = 0; i < imagen.width*imagen.height; ++i) {
        RGB pmapeado = imagen.RGBes.at(i);
        pmapeado.r= pmapeado.r*c/newM;
        pmapeado.g= pmapeado.g*c/newM;
        pmapeado.b= pmapeado.b*c/newM;
        salida <<std::setprecision(0)<<fixed<< pmapeado.r << " " << pmapeado.g << " " << pmapeado.b << "\t";
        if (i%imagen.width==0){
            salida << endl;
        }
    }
    salida.close();  

}

//*********************************************    BMP    ************************************************************** */

Image FileParser::readfileBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo BMP\n";
        exit(EXIT_FAILURE);
    }
    // Leer los primeros 2 bytes para comprobar si es un archivo BMP ('BM')
    char bmpType[2];
    file.read(bmpType, 2);
    if (bmpType[0] != 'B' || bmpType[1] != 'M') {
        cout << "El archivo no es un BMP válido\n";
        file.close();
        exit(EXIT_FAILURE);
    }
    // Saltar los siguientes 8 bytes (tamaño del archivo y campos reservados)
    file.ignore(8);
    // Leer el offset donde comienzan los datos de los píxeles (en los bytes 10-13)
    int offsetData = 0;
    for (int i = 0; i < 4; ++i) {
        offsetData |= (file.get() << (i * 8));
    }
    // Saltar hasta la información de ancho y alto en el encabezado (bytes 18-25)
    file.ignore(4);  // Saltar tamaño del encabezado
    int width = 0, height = 0;
    for (int i = 0; i < 4; ++i) {
        width |= (file.get() << (i * 8));  // Leer ancho
    }
    for (int i = 0; i < 4; ++i) {
        height |= (file.get() << (i * 8));  // Leer altura
    }
    // Leer los bits por píxel en los bytes 28-29
    file.ignore(2);  // Saltar a los bits por píxel
    int bitCount = 0;
    bitCount = file.get();        // Leer primer byte
    bitCount |= (file.get() << 8); // Leer segundo byte
    // Verificar que el BMP sea de 24 bits por píxel (RGB)
    if (bitCount != 24) {
        cout << "El archivo BMP no es de 24 bits por píxel\n";
        file.close();
        exit(EXIT_FAILURE);
    }
    // Saltar al comienzo de los datos de píxeles
    file.seekg(offsetData, std::ios::beg);
    // Crear la imagen
    Image imagen(width, std::abs(height));
    // Calcular el padding (cada fila debe ser múltiplo de 4 bytes)
    int rowPadding = (4 - (width * 3) % 4) % 4;
    // Leer los píxeles en formato BGR
    for (int y = 0; y < std::abs(height); ++y) {
        for (int x = 0; x < width; ++x) {
            RGB p;
            p.b = file.get(); // Leer componente azul
            p.g = file.get(); // Leer componente verde
            p.r = file.get(); // Leer componente rojo
            imagen.RGBes.push_back(p);
        }
        file.ignore(rowPadding); // Saltar el padding al final de la fila
    }

    file.close();
    return imagen;
}

void FileParser::writefileBMP(const Image& imagen, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo BMP para escribir\n";
        exit(EXIT_FAILURE);
    }

    // Escribir los primeros 2 bytes para indicar que es un archivo BMP ('BM')
    file.put('B');
    file.put('M');
    // Calcular el tamaño del archivo
    int fileSize = 54 + imagen.width * imagen.height * 3;
    file.put(fileSize & 0xFF);
    file.put((fileSize >> 8) & 0xFF);
    file.put((fileSize >> 16) & 0xFF);
    file.put((fileSize >> 24) & 0xFF);
    // Escribir los 4 bytes reservados (pueden ser 0)
    file.put(0);
    file.put(0);
    file.put(0);
    file.put(0);
    // Escribir el offset donde comienzan los datos de los píxeles (54 bytes)
    int offsetData = 54;
    file.put(offsetData & 0xFF);
    file.put((offsetData >> 8) & 0xFF);
    file.put((offsetData >> 16) & 0xFF);
    file.put((offsetData >> 24) & 0xFF);
    // Escribir la cabecera de información de la imagen (40 bytes)
    int infoHeaderSize = 40;
    file.put(infoHeaderSize & 0xFF);
    file.put((infoHeaderSize >> 8) & 0xFF);
    file.put((infoHeaderSize >> 16) & 0xFF);
    file.put((infoHeaderSize >> 24) & 0xFF);
    // Escribir el ancho y alto de la imagen
    file.put(imagen.width & 0xFF);
    file.put((imagen.width >> 8) & 0xFF);
    file.put((imagen.width >> 16) & 0xFF);
    file.put((imagen.width >> 24) & 0xFF);
    int height = -imagen.height; // Altura negativa para BMP "normal"
    file.put(height & 0xFF);
    file.put((height >> 8) & 0xFF);
    file.put((height >> 16) & 0xFF);
    file.put((height >> 24) & 0xFF);
   // Escribir los planos y bits por píxel (24 bits)
    file.put(1);
    file.put(0);
    file.put(24);
    file.put(0);
    // Escribir los campos restantes de la cabecera (compresión, tamaño de imagen, etc.)
    for (int i = 0; i < 24; ++i) {
        file.put(0);
    }
    // Calcular el padding
    int rowPadding = (4 - (imagen.width * 3) % 4) % 4;

    // Escribir los píxeles en formato BGR
    for (int y = 0; y < imagen.height; ++y) {
        for (int x = 0; x < imagen.width; ++x) {
            RGB p = imagen.RGBes[y * imagen.width + x];
            file.put(p.b);
            file.put(p.g);
            file.put(p.r);
        }
        // Escribir el padding al final de cada fila
        for (int i = 0; i < rowPadding; ++i) {
            file.put(0);
        }
    }
    file.close();
}

//****************************************************************************************************************** */



//*********************************************    OBJ    ************************************************************** */
TriangleMesh FileParser::readObj(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string line;
    std::vector<Point> vertices; // Assuming Point is a class for 3D points
    std::vector<Triangle*> triangles;
    Material material; // Assuming you have a default material or you can set it as needed

    while (std::getline(file, line)) {

        // Ignorar comentarios
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;


        if (prefix == "v") {
            // Vertex
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (prefix == "f") {
            // Face
            int v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            // Create a new Triangle and add it to the list
            triangles.push_back(new Triangle(vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1], material));
        }
    }

    cout<<"obj leido"<<endl;
    cout<<triangles.size()<<endl;

    file.close();

    // Create the TriangleMesh with the list of triangles and the material
    TriangleMesh triangleMesh(triangles, material);
    cout<<"Triangle mesh creado"<<endl;
    return triangleMesh;
}