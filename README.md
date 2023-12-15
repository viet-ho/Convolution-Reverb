# Convolution-Reverb

Convolution-Reverb is a command-line program developed for processing audio signals using convolution reverb techniques. This tool enables users to convolve a "dry" audio recording with an impulse response of an acoustical space, such as a concert hall, creating a sound file where the instrument appears to be playing in that specific environment.

## Features

- Audio Convolution: Apply natural-sounding reverberation to recorded sounds.
- Command-line Interface: Easy to use and integrate into existing workflows.
- Optimized Performance: Algorithm and compiler-level optimizations for efficient processing.
- File Format Support: Initially supports monophonic 16-bit 44.1 kHz WAV files. Bonus features include handling stereo files and additional formats like .aiff and .snd.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/viet-ho/Convolution-Reverb.git
   ```
2. Ensure you have a compatible environment (C, C++, Objective-C, supported by gcc compiler).

## Usage

To use Convolution-Reverb, run the following command:
```bash
convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>
```
- inputfile.wav is the dry recording.
- IRfile.wav is the impulse response.
- outputfile.wav is the convolved signal output.

## Development Stages

1. Baseline Program: Time-domain convolution.
2. Algorithm-Based Optimization: Frequency-domain convolution using FFT.
3. Compiler-Level Optimization: Performance tuning through compiler optimizations and manual code adjustments.
