  //  Copyright MIT License 2025 VL_PLAY Games


#include "../include/recorder.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <string>

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 512
#define SECONDS_OF_SILENCE 2
#define SILENCE_THRESHOLD_DEFAULT 500

struct RecorderData {
    std::vector<int16_t> recordedSamples;
    int silenceSamples = 0;
    bool recording = true;
};

Recorder::Recorder() {}

Recorder::~Recorder() {
    Pa_Terminate();
}

void Recorder::setMicrophone(const std::string& name) {
    microphone_name = name;
}

PaDeviceIndex Recorder::findInputDeviceByName(const std::string& name) {
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            if (name == deviceInfo->name) {
                return i;
            }
        }
    }
    return paNoDevice;
}

static int recordCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData) {
    RecorderData* data = (RecorderData*)userData;
    const int16_t* input = (const int16_t*)inputBuffer;

    double rms = 0.0;
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        rms += input[i] * input[i];
    }
    rms = sqrt(rms / framesPerBuffer);

    if (rms > SILENCE_THRESHOLD_DEFAULT) {
        data->silenceSamples = 0;
    }
    else {
        data->silenceSamples += framesPerBuffer;
        if (data->silenceSamples >= SAMPLE_RATE * SECONDS_OF_SILENCE) {
            data->recording = false;
            return paComplete;
        }
    }

    data->recordedSamples.insert(data->recordedSamples.end(), input, input + framesPerBuffer);
    return paContinue;
}

bool Recorder::record(const std::string& wavPath, int silenceThreshold) {
    Pa_Initialize();

    PaDeviceIndex deviceIndex = findInputDeviceByName(microphone_name);
    if (deviceIndex == paNoDevice) {
        std::cerr << "Microphone not found: " << microphone_name << std::endl;
        return false;
    }

    RecorderData data;
    PaStream* stream;

    PaStreamParameters inputParameters;
    inputParameters.device = deviceIndex;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&stream, &inputParameters, nullptr, SAMPLE_RATE,
        FRAMES_PER_BUFFER, paClipOff, recordCallback, &data);
    if (err != paNoError) {
        std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    Pa_StartStream(stream);
    while (data.recording) {
        Pa_Sleep(100);
    }
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    // Save to .wav
    std::ofstream file(wavPath, std::ios::binary);
    int32_t dataChunkSize = data.recordedSamples.size() * sizeof(int16_t);
    int32_t chunkSize = 36 + dataChunkSize;
    int32_t fmtChunkSize = 16;
    int16_t audioFormat = 1;
    int16_t numChannels = 1;
    int32_t sampleRate = SAMPLE_RATE;
    int32_t byteRate = sampleRate * sizeof(int16_t);
    int16_t blockAlign = sizeof(int16_t);
    int16_t bitsPerSample = 16;

    file.write("RIFF", 4);
    file.write((char*)&chunkSize, 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    file.write((char*)&fmtChunkSize, 4);
    file.write((char*)&audioFormat, 2);
    file.write((char*)&numChannels, 2);
    file.write((char*)&sampleRate, 4);
    file.write((char*)&byteRate, 4);
    file.write((char*)&blockAlign, 2);
    file.write((char*)&bitsPerSample, 2);
    file.write("data", 4);
    file.write((char*)&dataChunkSize, 4);
    file.write((char*)data.recordedSamples.data(), dataChunkSize);
    file.close();

    return true;
}

void Recorder::listAvailableMicrophones() const {
    Pa_Initialize();
    int numDevices = Pa_GetDeviceCount();

    if (numDevices < 0) {
        std::cerr << "ERROR: Pa_GetDeviceCount returned " \
            << numDevices << std::endl;
        return;
    }

    std::cout << "Available input devices:\n";
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            std::cout << " - " << deviceInfo->name << std::endl;
        }
    }

    Pa_Terminate();
}
