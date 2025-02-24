#pragma once

#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <atomic>

class Recorder {
public:
    // Конструктор с параметром имени микрофона, по умолчанию указывает на "Microphone (2- USB PnP Audio Device)"
    Recorder(const std::string& microphone_name = "Microphone (2- USB PnP Audio Device)");

    // Метод для записи аудио, с параметрами: имя файла и длительность записи (по умолчанию 5 секунд)
    void record(const std::string& filename, int duration = 5);

private:
    std::string microphone_name;       // Имя микрофона
    std::atomic<bool> stopRecording;   // Атомарная переменная для остановки записи
};

#endif // RECORDER_H
