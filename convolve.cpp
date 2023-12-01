#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <WAVEFile.h>

using namespace std;

void convolve(double x[], int N, double h[], int M, double y[], int P);
void createOutputFile(char *filename);
void shortToDouble(WAVEFile* waveFile, double doubleArray[]);

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

    cout << "\nInput Size: " << inputfile->signalSize << ", Impulse Size: " << IRfile->signalSize << '\n';


}

void convolve(double x[], int N, double h[], int M, double y[], int P) {

    int n, m;

    if (P != (N + M - 1)) {
        printf("Output signal vector is the wrong size\n");
        printf("It is %-d, but should be %-d\n", P, (N + M - 1));
        printf("Aborting convolution\n");
        return;
    }

    for (n = 0; n < P; n++) {
        y[n] = 0.0;
    }

    for (n = 0; n < N; n++) {
        for (m = 0; m < M; m++) {
            y[n+m] += x[n] * h[m];
        }
    }

}

void createOutputFile(char *filename) {

    double* input_signal = new double[inputfile->signalSize];
    double* IR_signal = new double[IRfile->signalSize];
    int output_size = inputfile->signalSize + IRfile->signalSize - 1;
    short* output_signal = new short[output_size];
    double* output_signal_double = new double[output_size];

}

void shortToDouble(WAVEFile* waveFile, double doubleArray[]) {
    
    for (int i = 0; i < (waveFile->signalSize); i++) {
        doubleArray[i] = ((double) waveFile->signal[i])/32678.0;
    }

}