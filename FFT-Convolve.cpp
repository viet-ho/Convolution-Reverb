#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <math.h>
#include "WAVEFile.h"
#define SWAP(a, b) \
    tempr = (a);   \
    (a) = (b);     \
    (b) = tempr

using namespace std;

void shortToDouble(WAVEFile *waveFile, double doubleArray[]);
void writeWAVEFileHeader(int numChannels, int numSamples, int bitsPerSample, int sampleRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *outputFile);
size_t fwriteShortLSB(short data, FILE *outputFile);
void four1(double data[], unsigned long nn, int isign);

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        cerr << "Usage: ./convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>\n";
        return EXIT_FAILURE;
    }

    WAVEFile *inputfile = new WAVEFile();
    WAVEFile *IRfile = new WAVEFile();

    char *inputFileName = argv[1];
    char *IRFileName = argv[2];
    char *outputFileName = argv[3];

    inputfile->readWAVEFile(inputFileName);
    IRfile->readWAVEFile(IRFileName);

    cout << "Input Size: " << inputfile->signalSize << ", Impulse Size: " << IRfile->signalSize << '\n';

    double *input_signal = new double[inputfile->signalSize];
    double *IR_signal = new double[IRfile->signalSize];
    shortToDouble(inputfile, input_signal);
    shortToDouble(IRfile, IR_signal);

    int maxFileSize = (inputfile->signalSize <= IRfile->signalSize) ? IRfile->signalSize : inputfile->signalSize;
    int powerOfTwo = 1;
    while (powerOfTwo < maxFileSize)
    {
        powerOfTwo *= 2;
    }

    double *freq_input_signal = new double[powerOfTwo * 2];
    double *freq_IR_signal = new double[powerOfTwo * 2];

    for (int i = 0; i < powerOfTwo * 2; i++)
    {
        freq_input_signal[i] = 0.0;
        freq_IR_signal[i] = 0.0;
    }

    for (int i = 0; i < (inputfile->signalSize); i++)
    {
        freq_input_signal[i * 2] = input_signal[i];
    }

    for (int i = 0; i < (IRfile->signalSize); i++)
    {
        freq_IR_signal[i * 2] = IR_signal[i];
    }

    four1(freq_input_signal-1, powerOfTwo, 1);
    four1(freq_IR_signal-1, powerOfTwo, 1);
}

void shortToDouble(WAVEFile *waveFile, double doubleArray[])
{
    for (int i = 0; i < (waveFile->signalSize); i++)
    {
        doubleArray[i] = ((double)waveFile->signal[i]) / 32678.0;
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

void four1(double data[], unsigned long nn, int isign)
{
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;

    for (i = 1; i < n; i += 2)
    {
        if (j > i)
        {
            SWAP(data[j], data[i]);
            SWAP(data[j + 1], data[i + 1]);
        }
        m = nn;
        while (m >= 2 && j > m)
        {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    mmax = 2;
    while (n > mmax)
    {
        istep = mmax << 1;
        theta = isign * (6.28318530717959 / mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2)
        {
            for (i = m; i <= n; i += istep)
            {
                j = i + mmax;
                tempr = wr * data[j] - wi * data[j + 1];
                tempi = wr * data[j + 1] + wi * data[j];
                data[j] = data[i] - tempr;
                data[j + 1] = data[i + 1] - tempi;
                data[i] += tempr;
                data[i + 1] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}