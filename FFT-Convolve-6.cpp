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

void createOutputFile(char *filename);
void shortToDouble(WAVEFile *waveFile, double doubleArray[]);
void writeWAVEFileHeader(int numChannels, int numSamples, int bitsPerSample, int sampleRate, FILE *outputFile);
inline size_t fwriteIntLSB(int data, FILE *outputFile);
inline size_t fwriteShortLSB(short data, FILE *outputFile);
void four1(double data[], unsigned long nn, int isign);
void adjustOutputSignal(WAVEFile *waveFile, double *output_signal, int output_size);
void convolve(double *freq_input_signal, double *freq_IR_signal, double *freq_output_signal, int length);

WAVEFile *inputfile = new WAVEFile();
WAVEFile *IRfile = new WAVEFile();

int main(int argc, char *argv[])
{
    clock_t startTime;
    clock_t endTime;
    startTime = clock();

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

    endTime = clock();
    double time = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
    printf("The process was done in %.3f seconds!\n", time);

    return 0;
}

void createOutputFile(char *filename)
{
    int output_size = inputfile->signalSize + IRfile->signalSize - 1;
    double *output_signal = new double[output_size];
    double *input_signal = new double[inputfile->signalSize];
    double *IR_signal = new double[IRfile->signalSize];
    shortToDouble(inputfile, input_signal);
    shortToDouble(IRfile, IR_signal);

    int maxFileSize = (inputfile->signalSize <= IRfile->signalSize) ? IRfile->signalSize : inputfile->signalSize;
    int powerOfTwo = 1;
    while (powerOfTwo < maxFileSize)
    {
        // Strength Reduction - Optimization 6
        powerOfTwo <<= 1;
    }

    // Avoiding Recomputation - Optimization 3
    int maxLength = powerOfTwo * 2;

    double *freq_input_signal = new double[maxLength];
    double *freq_IR_signal = new double[maxLength];
    double *freq_output_signal = new double[maxLength];

    // Loop Unrolling - Optimization 1
    for (int i = 0; i < maxLength; i += 2)
    {
        freq_input_signal[i] = 0.0;
        freq_IR_signal[i] = 0.0;

        freq_input_signal[i + 1] = 0.0;
        freq_IR_signal[i + 1] = 0.0;
    }

    for (int i = 0; i < (inputfile->signalSize); i++)
    {
        // Strength Reduction - Optimization 6
        freq_input_signal[i<<1] = input_signal[i];
    }

    for (int i = 0; i < (IRfile->signalSize); i++)
    {
        // Strength Reduction - Optimization 6
        freq_IR_signal[i<<1] = IR_signal[i];
    }

    four1(freq_input_signal - 1, powerOfTwo, 1);
    four1(freq_IR_signal - 1, powerOfTwo, 1);

    convolve(freq_input_signal, freq_IR_signal, freq_output_signal, maxLength);

    four1(freq_output_signal - 1, powerOfTwo, -1);
    for (int i = 0; i < output_size; i++)
    {
        // Strength Reduction - Optimization 6
        output_signal[i] = freq_output_signal[i<<1];
    }
    adjustOutputSignal(inputfile, output_signal, output_size);

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
        fwriteShortLSB(static_cast<short>(output_signal[i]), outputfile);
    }
    printf("End writing!\n");

    fclose(outputfile);
}

void shortToDouble(WAVEFile *waveFile, double doubleArray[])
{
    for (int i = 0; i < (waveFile->signalSize); i++)
    {
        doubleArray[i] = ((double)waveFile->signal[i]) / 32678.0;
    }
}

void convolve(double *freq_input_signal, double *freq_IR_signal, double *freq_output_signal, int length)
{
    printf("Start convolution...\n");

    // Optimizing Memory Access - Optimization 4
    for (int i = 0; i < length; i += 2)
    {
        double inputReal = freq_input_signal[i];
        double inputImag = freq_input_signal[i + 1];
        double irReal = freq_IR_signal[i];
        double irImag = freq_IR_signal[i + 1];

        freq_output_signal[i] = (inputReal * irReal) - (inputImag * irImag);
        freq_output_signal[i + 1] = (inputImag * irReal) + (inputReal * irImag);
    }
    printf("End convolution!\n");
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

// Inline Functions - Optimization 2
inline size_t fwriteIntLSB(int data, FILE *outputFile)
{
    unsigned char array[4];
    array[3] = (unsigned char)((data >> 24) & 0xFF);
    array[2] = (unsigned char)((data >> 16) & 0xFF);
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 4, outputFile);
}

// Inline Functions - Optimization 2
inline size_t fwriteShortLSB(short data, FILE *outputFile)
{
    unsigned char array[2];
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 2, outputFile);
}

void adjustOutputSignal(WAVEFile *waveFile, double *output_signal, int output_size)
{

    double signalInputMax = 0.0;
    double signalOutputMax = 0.0;

    for (int i = 0; i < output_size; i++)
    {
        // Optimizing Conditional Statements - Optimization 5
        signalInputMax = (waveFile->signal[i] > signalInputMax) ? waveFile->signal[i] : signalInputMax;
        signalOutputMax = (output_signal[i] > signalOutputMax) ? output_signal[i] : signalOutputMax;
    }

    double adjustFactor = signalInputMax / signalOutputMax;

    // Loop Unrolling - Optimization 1
    int i = 0;
    for (; i <= output_size - 4; i += 4)
    {
        output_signal[i] *= adjustFactor;
        output_signal[i + 1] *= adjustFactor;
        output_signal[i + 2] *= adjustFactor;
        output_signal[i + 3] *= adjustFactor;
    }

    for (; i < output_size; i++)
    {
        output_signal[i] *= adjustFactor;
    }
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
            // Loop Unrolling - Optimization 1
            for (i = m; i <= n; i += istep * 2)
            {
                j = i + mmax;
                tempr = wr * data[j] - wi * data[j + 1];
                tempi = wr * data[j + 1] + wi * data[j];
                data[j] = data[i] - tempr;
                data[j + 1] = data[i + 1] - tempi;
                data[i] += tempr;
                data[i + 1] += tempi;

                if (i + istep <= n)
                {
                    int j_unrolled = i + istep + mmax;
                    double tempr_unrolled = wr * data[j_unrolled] - wi * data[j_unrolled + 1];
                    double tempi_unrolled = wr * data[j_unrolled + 1] + wi * data[j_unrolled];
                    data[j_unrolled] = data[i + istep] - tempr_unrolled;
                    data[j_unrolled + 1] = data[i + istep + 1] - tempi_unrolled;
                    data[i + istep] += tempr_unrolled;
                    data[i + istep + 1] += tempi_unrolled;
                }
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}