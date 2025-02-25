#pragma once
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <string>
#include "log.h"

class Recognizer {
public:
    // Конструктор с параметрами пути к Whisper и модели, а также объектом Log
    Recognizer(const std::string& whisper_path, const std::string& model_path, Log& log);

    // Метод для распознавания речи из аудиофайла
    std::string recognize(const std::string& filename);

private:
    std::string whisper_path;  // Путь к исполняемому файлу Whisper
    std::string model_path;    // Путь к модели Whisper
    Log& log;                  // Ссылка на объект Log для записи в лог
};

#endif // RECOGNIZER_H