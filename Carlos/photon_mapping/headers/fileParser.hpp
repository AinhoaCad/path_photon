#ifndef FILEPARSER_HPP
#define FILEPARSER_HPP
#include "image.hpp"
#include "triangleMesh.hpp"
class FileParser{
    public:
    static Image readfilePPM(string ficheroEntrada);
    static void writefilePPM(Image imagen,string fichSalida);
    static Image readfileBMP(const std::string& filename);
    static void writefileBMP(const Image& imagen, const std::string& filename);
    static TriangleMesh readObj(const std::string& filename);

};
#endif