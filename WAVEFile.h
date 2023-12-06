/*
 * Name: Viet Ho
 * UCID: 30122283
 * Date: Dec. 5th, 2023
 * Class Description: Header file for WAVE file and reading data from file.
 */

#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <iostream>
#include <fstream>
#include <cstring>

class WAVEFile
{

// WAVE file structure
public:
    char chunkID[4];
    int chunkSize;
    char format[4];
    char subChunk1ID[4];
    int subChunk1Size;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
    char subChunk2ID[4];
    int subChunk2Size;
    char *audioData;
    short *signal;
    int signalSize;

    // Method to read data from WAVE file
    void readWAVEFile(const char *fileName)
    {
        std::ifstream file(fileName, std::ios::binary);

        file.read(chunkID, sizeof(chunkID));
        file.read(reinterpret_cast<char *>(&chunkSize), sizeof(chunkSize));
        file.read(format, sizeof(format));

        file.read(subChunk1ID, sizeof(subChunk1ID));
        file.read(reinterpret_cast<char *>(&subChunk1Size), sizeof(subChunk1Size));
        file.read(reinterpret_cast<char *>(&audioFormat), sizeof(audioFormat));
        file.read(reinterpret_cast<char *>(&numChannels), sizeof(numChannels));
        file.read(reinterpret_cast<char *>(&sampleRate), sizeof(sampleRate));
        file.read(reinterpret_cast<char *>(&byteRate), sizeof(byteRate));
        file.read(reinterpret_cast<char *>(&blockAlign), sizeof(blockAlign));
        file.read(reinterpret_cast<char *>(&bitsPerSample), sizeof(bitsPerSample));

        if (subChunk1Size == 18)
        {
            file.ignore(sizeof(short));
        }

        file.read(subChunk2ID, sizeof(subChunk2ID));
        file.read(reinterpret_cast<char *>(&subChunk2Size), sizeof(subChunk2Size));

        audioData = new char[subChunk2Size];
        file.read(audioData, subChunk2Size);

        file.close();

        dataToSignal();
    }

private:
    // Method to convert data to signal based on bitsPerSample
    void dataToSignal()
    {
        signal = nullptr;
        if (bitsPerSample == 8)
        {
            signalSize = subChunk2Size;
            signal = new short[signalSize];
            for (int i = 0; i < subChunk2Size; i++)
            {
                signal[i] = static_cast<short>(static_cast<unsigned char>(audioData[i]));
            }
        }
        else
        {
            signalSize = subChunk2Size / 2;
            signal = new short[signalSize];
            for (int i = 0; i < subChunk2Size; i += 2)
            {
                short tempSignal = static_cast<short>(static_cast<unsigned char>(audioData[i]));
                tempSignal |= static_cast<short>(static_cast<unsigned char>(audioData[i + 1])) << 8;
                signal[i / 2] = tempSignal;
            }
        }
    }
};

#endif