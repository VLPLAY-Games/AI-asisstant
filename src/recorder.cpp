// Copyright MIT License 2025 VL_PLAY Games


#include "../include/recorder.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
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

Recorder::Recorder() : stopRequested(false) {
    std::cout << "\n###############################\n";
    std::cout << "     Initializing PortAudio    \n";
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " \
            << Pa_GetErrorText(err) << " (Error code: " << err << ")\n";
    } else {
        std::cout << "PortAudio initialized successfully.\n";
    }
    std::cout << "###############################\n\n";
}

Recorder::~Recorder() {
    Pa_Terminate();
}

void Recorder::setMicrophone(const std::string &name) {
    microphone_name = name;
    std::cout << "Microphone set to: " << name << "\n";
}

void Recorder::stopRecording() {
    stopRequested = true;  // Устанавливаем флаг для остановки записи
}

PaDeviceIndex Recorder::findInputDeviceByName(const std::string &name) {
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        std::cerr << "PortAudio error: " \
            << Pa_GetErrorText(numDevices) << " (Error code: " \
            << numDevices << ")\n";
        return paNoDevice;
    }
    std::cout << "Found " << numDevices << " devices.\n";
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            std::cout << "Device " << i << ": " \
                << deviceInfo->name << " (Input channels: " \
                << deviceInfo->maxInputChannels << ")\n";
            if (name == deviceInfo->name) {
                std::cout << "Selected device: " \
                    << deviceInfo->name << " (Index: " << i << ")\n";
                return i;
            }
        }
    }
    std::cerr << "Microphone not found: " << name << "\n";
    return paNoDevice;
}

static int recordCallback(const void *inputBuffer,
                          void *outputBuffer,
                          unsigned int64_t framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData) {
    RecorderData *data = (RecorderData *) userData;
    const int16_t *input = (const int16_t *) inputBuffer;

    if (!input) {
        std::cerr << "No input buffer received in recordCallback.\n";
        return paContinue;
    }

    double rms = 0.0;
    for (unsigned int64_t i = 0; i < framesPerBuffer; ++i) {
        rms += input[i] * input[i];
    }
    rms = sqrt(rms / framesPerBuffer);
    std::cout << "RMS level: " << rms << "\n";

    if (rms > SILENCE_THRESHOLD_DEFAULT) {
        data->silenceSamples = 0;
    } else {
        data->silenceSamples += framesPerBuffer;
        std::cout << "Silence samples: " << data->silenceSamples \
            << " (Threshold: " << SAMPLE_RATE * SECONDS_OF_SILENCE << ")\n";
        if (data->silenceSamples >= SAMPLE_RATE * SECONDS_OF_SILENCE) {
            data->recording = false;
            return paComplete;
        }
    }

    data->recordedSamples.insert(data->recordedSamples.end(), \
        input, input + framesPerBuffer);
    return paContinue;
}

bool Recorder::record(const std::string &wavPath, int silenceThreshold) {
    stopRequested = false;  // Сбрасываем флаг остановки перед началом записи
    PaDeviceIndex deviceIndex = findInputDeviceByName(microphone_name);
    if (deviceIndex == paNoDevice) {
        std::cerr << "Failed to find microphone: " << microphone_name << "\n";
        return false;
    }

    RecorderData data;
    PaStream *stream = nullptr;

    PaStreamParameters inputParameters;
    inputParameters.device = deviceIndex;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = \
        Pa_GetDeviceInfo(deviceIndex)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&stream,
                                &inputParameters,
                                nullptr,
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,
                                paClipOff,
                                recordCallback,
                                &data);
    if (err != paNoError) {
        std::cerr << "Failed to open stream: " \
            << Pa_GetErrorText(err) << " (Error code: " << err << ")\n";
        return false;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start stream: " \
            << Pa_GetErrorText(err) << " (Error code: " << err << ")\n";
        Pa_CloseStream(stream);
        return false;
    }

    while (data.recording && !stopRequested) {
        Pa_Sleep(100);
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to stop stream: " \
            << Pa_GetErrorText(err) << " (Error code: " << err << ")\n";
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to close stream: " \
            << Pa_GetErrorText(err) << " (Error code: " << err << ")\n";
    }

    if (data.recordedSamples.empty()) {
        std::cerr << "No audio data recorded.\n";
        return false;
    }

    std::ofstream file(wavPath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file for writing: " << wavPath << "\n";
        return false;
    }

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
    file.write((char *) &chunkSize, 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    file.write((char *) &fmtChunkSize, 4);
    file.write((char *) &audioFormat, 2);
    file.write((char *) &numChannels, 2);
    file.write((char *) &sampleRate, 4);
    file.write((char *) &byteRate, 4);
    file.write((char *) &blockAlign, 2);
    file.write((char *) &bitsPerSample, 2);
    file.write("data", 4);
    file.write((char *) &dataChunkSize, 4);
    file.write((char *) data.recordedSamples.data(), dataChunkSize);
    file.close();

    std::cout << "Recording saved to: " << wavPath << "\n";
    return true;
}

std::vector<std::string> Recorder::listAvailableMicrophones() const {
    std::vector<std::string> microphones;

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        std::cerr << "ERROR: Pa_GetDeviceCount returned " \
            << numDevices << ": " << Pa_GetErrorText(numDevices) << "\n";
        return microphones;
    }

    std::cout << "Listing available microphones:\n";
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            std::cout << " - " << deviceInfo->name << " (Index: " << i << ")\n";
            microphones.push_back(deviceInfo->name);
        }
    }

    if (microphones.empty()) {
        std::cerr << "No input devices found.\n";
    }

    return microphones;
}
