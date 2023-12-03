#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include "WAVEFile.h"

using namespace std;

void convolve(double *x, int N, double *h, int M, double *y, int P);
void createOutputFile(char *filename);
void shortToDouble(WAVEFile *waveFile, double *doubleArray);
void adjustOutputSignal(WAVEFile *waveFile, double *output_signal, int output_size);
void writeWAVEFileHeader(int numChannels, int numSamples, int bitsPerSample, int sampleRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *outputFile);
size_t fwriteShortLSB(short data, FILE *outputFile);

WAVEFile *inputfile = new WAVEFile();
WAVEFile *IRfile = new WAVEFile();

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        cerr << "Usage: ./convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>\n";
        return EXIT_FAILURE;
    }

    char *inputFileName = argv[1];
    char *IRFileName = argv[2];
    char *outputFileName = argv[3];

    inputfile->readWAVEFile(inputFileName);
    IRfile->readWAVEFile(IRFileName);

    cout << "Input Size: " << inputfile->signalSize << ", Impulse Size: " << IRfile->signalSize << '\n';

    createOutputFile(outputFileName);
    return 0;
}

void convolve(double *x, int N, double *h, int M, double *y, int P)
{

    int n, m;

    if (P != (N + M - 1))
    {
        printf("Output signal vector is the wrong size\n");
        printf("It is %-d, but should be %-d\n", P, (N + M - 1));
        printf("Aborting convolution\n");
        return;
    }

    for (n = 0; n < P; n++)
    {
        y[n] = 0.0;
    }

    for (n = 0; n < N; n++)
    {
        for (m = 0; m < M; m++)
        {
            y[n + m] += x[n] * h[m];
        }
    }
}

void createOutputFile(char *filename)
{

    double *input_signal = new double[inputfile->signalSize];
    double *IR_signal = new double[IRfile->signalSize];
    int output_size = inputfile->signalSize + IRfile->signalSize - 1;
    double *output_signal = new double[output_size];
    short *output_signal_short = new short[output_size];
    clock_t startTime;
    clock_t endTime;

    shortToDouble(inputfile, input_signal);
    shortToDouble(IRfile, IR_signal);

    printf("Start convolution...\n");
    startTime = clock();
    convolve(input_signal, inputfile->signalSize, IR_signal, IRfile->signalSize, output_signal, output_size);
    endTime = clock();
    double time = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
    printf("End convolution!\n");
    printf("The convolution was done in %.2f seconds!\n", time);

    adjustOutputSignal(inputfile, output_signal, output_size);
    for (int i = 0; i < output_size; i++)
    {
        output_signal_short[i] = static_cast<short>(output_signal[i]);
    }

    FILE *outputfile = fopen(filename, "wb");
    if (outputfile == NULL)
    {
        fprintf(stderr, "File %s cannot be opened for writing\n", filename);
        return;
    }

    printf("Start writing header and signal data to output file...\n");
    writeWAVEFileHeader(inputfile->numChannels, output_size, inputfile->bitsPerSample, inputfile->sampleRate, outputfile);
    for (int i = 0; i < output_size; i++)
    {
        fwriteShortLSB(output_signal_short[i], outputfile);
    }
    printf("End writing!\n");

    fclose(outputfile);
}

void shortToDouble(WAVEFile *waveFile, double *doubleArray)
{

    for (int i = 0; i < (waveFile->signalSize); i++)
    {
        doubleArray[i] = ((double)waveFile->signal[i]) / 32678.0;
    }
}

void adjustOutputSignal(WAVEFile *waveFile, double *output_signal, int output_size)
{

    double signalInputMax = 0.0;
    double signalOutputMax = 0.0;

    for (int i = 0; i < output_size; i++)
    {
        if (signalInputMax < waveFile->signal[i])
        {
            signalInputMax = waveFile->signal[i];
        }

        if (signalOutputMax < output_signal[i])
        {
            signalOutputMax = output_signal[i];
        }
    }

    double adjustFactor = signalInputMax / signalOutputMax;

    for (int i = 0; i < output_size; i++)
    {
        output_signal[i] *= adjustFactor;
    }
}

void writeWAVEFileHeader(int numChannels, int numSamples, int bitsPerSample, int sampleRate, FILE *outputFile)
{

    int subChunk2Size = numChannels * numSamples * (bitsPerSample / 8);
    int chunkSize = subChunk2Size + 36;
    short blockAlign = numChannels * (bitsPerSample / 8);
    int byteRate = static_cast<int>(sampleRate * blockAlign);

    fputs("RIFF", outputFile);
    fwriteIntLSB(chunkSize, outputFile);
    fputs("WAVE", outputFile);
    fputs("fmt ", outputFile);
    fwriteIntLSB(16, outputFile);
    fwriteShortLSB(1, outputFile);
    fwriteShortLSB(numChannels, outputFile);
    fwriteIntLSB(sampleRate, outputFile);
    fwriteIntLSB(byteRate, outputFile);
    fwriteShortLSB(blockAlign, outputFile);
    fwriteShortLSB(bitsPerSample, outputFile);
    fputs("data", outputFile);
    fwriteIntLSB(subChunk2Size, outputFile);
}

size_t fwriteIntLSB(int data, FILE *outputFile)
{

    unsigned char array[4];
    array[3] = (unsigned char)((data >> 24) & 0xFF);
    array[2] = (unsigned char)((data >> 16) & 0xFF);
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 4, outputFile);
}

size_t fwriteShortLSB(short data, FILE *outputFile)
{

    unsigned char array[2];
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 2, outputFile);
}