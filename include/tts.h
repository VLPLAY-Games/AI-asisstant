#pragma once

#ifndef TTS_H
#define TTS_H

#include <sapi.h>
#include <iostream>
#include <string>

#pragma comment(lib, "sapi.lib")

class TextToSpeech {
public:
    TextToSpeech();
    ~TextToSpeech();
    void speak(const std::wstring& text);

private:
    bool initialized;
};

#endif // TTS_H
