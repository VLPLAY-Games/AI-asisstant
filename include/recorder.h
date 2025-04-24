  //  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef RECORDER_H
#define RECORDER_H
#include <string>
#include <vector>
#include <portaudio.h>

class Recorder {
 public:
    Recorder();
    ~Recorder();

    // Устанавливаем имя микрофона
    void setMicrophone(const std::string& name);

    // Основной метод записи
    bool record(const std::string& wavPath, int silenceThreshold);
    void listAvailableMicrophones() const;

 private:
    std::string microphone_name;
    PaDeviceIndex findInputDeviceByName(const std::string& name);
};

#endif
