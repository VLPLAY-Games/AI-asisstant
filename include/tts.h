//  Copyright MIT License 2025 VL_PLAY Games

#pragma once
#ifndef TTS_H
#define TTS_H

#include <string>
#include "log.h"

#ifdef _WIN32
#include <sapi.h>
#pragma comment(lib, "sapi.lib")
#endif

class TextToSpeech {
public:
    TextToSpeech(Log& log);
    ~TextToSpeech();

    void speak(const std::wstring& text);

private:
    bool initialized;
    Log& log;

#ifdef _WIN32
    // Windows-specific
#else
    // Linux-specific
    std::wstring escapeForShell(const std::wstring& input);
#endif
};

#endif  // TTS_H
