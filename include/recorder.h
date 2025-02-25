#pragma once

#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <atomic>
#include "log.h"

class Recorder {
public:
    // Конструктор с параметром имени микрофона и объектом Log
    Recorder(const std::string& microphone_name, Log& log);

    // Метод для записи аудио
    void record(const std::string& filename, int silence_db);

private:
    std::string microphone_name;       // Имя микрофона
    std::atomic<bool> stopRecording;   // Атомарная переменная для остановки записи
    Log& log;                          // Ссылка на объект Log
};

#endif // RECORDER_H