  //  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef TTS_H
#define TTS_H

#include <sapi.h>
#include <iostream>
#include <string>
#include "log.h"

#pragma comment(lib, "sapi.lib")

class TextToSpeech {
 public:
    // Конструктор с объектом Log
    TextToSpeech(Log& log);

    // Деструктор
    ~TextToSpeech();

    // Метод для воспроизведения текста
    void speak(const std::wstring& text);

 private:
    bool initialized;  // Флаг успешной инициализации
    Log& log;         // Ссылка на объект Log для записи в лог
};

#endif  // TTS_H
