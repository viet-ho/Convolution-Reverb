#include <iostream>
#include <fstream>
#include <WAVEFile.h>

using namespace std;

WAVEFile *inputfile = new WAVEFile();
WAVEFile *IRfile = new WAVEFile();

int main(int argc, char* argv[]){

    if (argc != 4) {
        cerr << "Usage: ./convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>\n";
        return EXIT_FAILURE;
    }

    char* inputFileName = argv[1];
    char* IRFileName = argv[2];
    char* outputFileName = argv[3];

    inputfile->readWAVEFile(inputFileName);
    IRfile->readWAVEFile(IRFileName);

}