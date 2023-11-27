#ifndef HEADERFILE_H
#define HEADERFILE_H

class WAVEFile
{

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
    char *data;
    short *signal;
    int signalSize;

public:
    void readWAVEFile(char* fileName);

};

#endif // HEADERFILE_H