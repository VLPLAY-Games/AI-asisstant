//#include "../include/recorder.h"
//#include <iostream>
//#include <cstdlib>
//
//Recorder::Recorder(const std::string& microphone_name)
//    : microphone_name(microphone_name) {
//}
//
//void Recorder::record(const std::string& filename, int duration) {
//    std::cout << "Listening...\n";
//
//    std::string command = "ffmpeg -f dshow -i audio=\"" + microphone_name +
//        "\" -t " + std::to_string(duration) +
//        " -ac 1 -ar 16000 " + filename + " -y -loglevel error";
//
//    std::system(command.c_str());
//    std::cout << "Done listening.\n";
//}


#define MINIAUDIO_IMPLEMENTATION
#include "../software/miniaudio/miniaudio.h"
#include "../include/recorder.h"
#include <iostream>
#include <thread>
#include <chrono>

// ������������
const int SAMPLE_RATE = 44100; // ������� ������������� (44.1 ���)
const int FRAME_SIZE = 1024;   // ������ ������
const float SILENCE_THRESHOLD = 0.0005f; // ����� ������ (0.05%)
const int SILENCE_DURATION = 3000; // ������������ ������ (3 �������)

// ���������� ����������
std::chrono::steady_clock::time_point lastSoundTime;
bool isRecordingGlobal = true;

// Callback-������� ��� ��������� �����������
void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    const float* input = (const float*)pInput;
    float rms = 0.0f;

    // ��������� RMS (������������������ ��������) ��� ����������� ���������
    for (ma_uint32 i = 0; i < frameCount; i++) {
        rms += input[i] * input[i];
    }
    rms = sqrt(rms / frameCount);

    // ���������� �����
    std::cout << "RMS: " << rms << " | Threshold: " << SILENCE_THRESHOLD << std::endl;

    // ���������, ��������� �� ��������� ����� ������
    if (rms > SILENCE_THRESHOLD) {
        lastSoundTime = std::chrono::steady_clock::now();
        std::cout << "Sound detected! Resetting silence timer." << std::endl;
    }
    else {
        auto now = std::chrono::steady_clock::now();
        auto silenceTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSoundTime).count();
        std::cout << "Silence time: " << silenceTime << " ms" << std::endl;
        if (silenceTime >= SILENCE_DURATION) {
            isRecordingGlobal = false; // ������������� ������
            std::cout << "Recording stopped due to silence." << std::endl;
        }
    }

    (void)pOutput; // �� ���������� �����
}

Recorder::Recorder(const std::string& microphone_name)
    : microphone_name(microphone_name), is_recording(false) {
    // �������������
}

Recorder::~Recorder() {
    stopRecording();
}

void Recorder::startRecording(const std::string& filename) {
    if (is_recording) return;
    is_recording = true;

    // ������������� ������� ���������� �����
    lastSoundTime = std::chrono::steady_clock::now();

    // ������ ������ � ��������� ������
    std::thread([this, filename]() {
        processAudioStream(filename);
        }).detach();
}

void Recorder::stopRecording() {
    is_recording = false;
}

bool Recorder::isRecording() const {
    return is_recording;
}

void Recorder::setOnSpeechStartCallback(const std::function<void()>& callback) {
    on_speech_start = callback;
}

void Recorder::setOnSpeechEndCallback(const std::function<void()>& callback) {
    on_speech_end = callback;
}

void Recorder::processAudioStream(const std::string& filename) {
    ma_result result;
    ma_device_config deviceConfig;
    ma_device device;

    // ������������� ����������
    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32; // 32-������ ������ � ��������� ������
    deviceConfig.capture.channels = 1;          // ����
    deviceConfig.sampleRate = SAMPLE_RATE;
    deviceConfig.dataCallback = audio_callback;

    result = ma_device_init(nullptr, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio device!" << std::endl;
        return;
    }

    // ������ ����������
    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start audio device!" << std::endl;
        ma_device_uninit(&device);
        return;
    }

    std::cout << "Recording to file: " << filename << std::endl;

    // �������� ��������� ������
    while (isRecordingGlobal) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ��������� ����������
    ma_device_stop(&device);
    ma_device_uninit(&device);

    std::cout << "Recording stopped." << std::endl;
}