#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include "WAVEFile.h"

using namespace std;

bool isValidWAVEFile(char *filename);
bool compareWAVEFiles(char *baseFileName, char *fftFileName);
bool compareHeaders(WAVEFile *baseFile, WAVEFile *fftFile);
bool compareData(WAVEFile *baseFile, WAVEFile *fftFile);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: ./RegressionTest <BaseOutputFile.wav> <FFTOutputFile.wav>\n";
        return EXIT_FAILURE;
    }

    char *baseFileName = argv[1];
    char *fftFileName = argv[2];

    if (!isValidWAVEFile(baseFileName) || !isValidWAVEFile(fftFileName))
    {
        cerr << "Error: Invalid file(s). Ensure both are WAV files." << endl;
        return EXIT_FAILURE;
    }

    if (!compareWAVEFiles(baseFileName, fftFileName))
    {
        cout << "Files are not equivalent." << endl;
    }
    else
    {
        cout << "Files are equivalent." << endl;
    }

    return EXIT_SUCCESS;
}

bool isValidWAVEFile(char *filename)
{
    size_t len = strlen(filename);

    const char *dotPosition = strrchr(filename, '.');
    if (!dotPosition || strcmp(dotPosition, ".wav") != 0)
    {
        cerr << "Invalid file extension for " << filename << endl;
        return false;
    }
    return true;
}

bool compareWAVEFiles(char *baseFileName, char *fftFileName)
{
    WAVEFile *baseFile = new WAVEFile();
    WAVEFile *fftFile = new WAVEFile();
    baseFile->readWAVEFile(baseFileName);
    fftFile->readWAVEFile(fftFileName);

    cout << "Comparing WAVE headers..." << endl;
    if (!compareHeaders(baseFile, fftFile))
    {
        cout << "WAVE headers differ." << endl;
        return false;
    }

    cout << "Comparing WAVE data..." << endl;
    return compareData(baseFile, fftFile);
}

bool compareHeaders(WAVEFile *baseFile, WAVEFile *fftFile)
{
    return (strcmp(baseFile->chunkID, fftFile->chunkID) == 0) &&
           (baseFile->chunkSize == fftFile->chunkSize) &&
           (strcmp(baseFile->format, fftFile->format) == 0) &&
           (strcmp(baseFile->subChunk1ID, fftFile->subChunk1ID) == 0) &&
           (baseFile->subChunk1Size == fftFile->subChunk1Size) &&
           (baseFile->audioFormat == fftFile->audioFormat) &&
           (baseFile->numChannels == fftFile->numChannels) &&
           (baseFile->sampleRate == fftFile->sampleRate) &&
           (baseFile->byteRate == fftFile->byteRate) &&
           (baseFile->blockAlign == fftFile->blockAlign) &&
           (baseFile->bitsPerSample == fftFile->bitsPerSample) &&
           (strcmp(baseFile->subChunk2ID, fftFile->subChunk2ID) == 0) &&
           (baseFile->subChunk2Size == fftFile->subChunk2Size);
}

bool compareData(WAVEFile *baseFile, WAVEFile *fftFile)
{
    if (baseFile->signalSize != fftFile->signalSize)
    {
        cout << "Signal sizes differ." << endl;
        return false;
    }

    for (int i = 0; i < baseFile->signalSize; ++i)
    {
        if (baseFile->signal[i] != fftFile->signal[i])
        {
            cout << "Data mismatch at position " << i << endl;
            return false;
        }
    }

    return true;
}
