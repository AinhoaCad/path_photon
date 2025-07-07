#include <iostream>
#include "Image.h"


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.ppm> <output.ppm>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    Image image;
    printf("Reading input file %s\n", inputFile.c_str());
    if (!image.readPPM(inputFile)) {
        std::cerr << "Error: Failed to read input file." << std::endl;
        return 1;
    }

    image.clampPixels();
    //image.ecualizacion();
    image.curvaGamma(0.8);
    printf("Writing output file %s\n", outputFile.c_str());
    if (!image.writePPM(outputFile)) {
        std::cerr << "Error: Failed to write output file." << std::endl;
        return 1;
    }

    std::cout << "Image successfully copied from " << inputFile << " to " << outputFile << std::endl;

    return 0;
}
