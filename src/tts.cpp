//  Copyright MIT License 2025 VL_PLAY Games

#include "../include/tts.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

TextToSpeech::TextToSpeech(Log &log)
    : initialized(false)
    , log(log)
#ifdef _WIN32
    , pVoice(nullptr)
#endif
{
    std::cout << "\n###############################\n";
    std::cout << "     Initializing TTS          \n";
    std::cout << "###############################\n\n";

#ifdef _WIN32
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        log.error("Failed to initialize COM library. HRESULT: " + \
            std::to_string(hr));
        return;
    }

    hr = CoCreateInstance(CLSID_SpVoice, nullptr, \
        CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    if (FAILED(hr)) {
        log.error("Failed to create SAPI voice instance. HRESULT: " + \
            std::to_string(hr));
        CoUninitialize();
        return;
    }

    initialized = true;
    log.info("TextToSpeech initialized successfully (Windows).");
#else
    initialized = true;
    log.info("TextToSpeech initialized successfully (Linux).");
#endif

    std::cout << "\n############################\n";
    std::cout << "        TTS Ready           \n";
    std::cout << "############################\n\n";
}

TextToSpeech::~TextToSpeech() {
#ifdef _WIN32
    if (pVoice) {
        pVoice->Release();
        pVoice = nullptr;
    }
    CoUninitialize();
#endif
    log.info("TextToSpeech destroyed.");
}

void TextToSpeech::speak(const std::wstring &text) {
    std::wcout << std::endl;
    if (!initialized) {
        log.error("TextToSpeech is not initialized. Cannot speak.");
        return;
    }

#ifdef _WIN32
    // Инициализируем COM для текущего потока
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != S_FALSE) {
        // S_FALSE означает, что COM уже инициализирован
        log.error("Failed to initialize COM in speak thread. HRESULT: " + \
            std::to_string(hr));
        return;
    }

    if (pVoice) {
        hr = pVoice->SetOutput(nullptr, TRUE);
        if (FAILED(hr)) {
            log.error("Failed to set SAPI output. HRESULT: " + \
                std::to_string(hr));
            CoUninitialize();
            return;
        }

        hr = pVoice->Speak(text.c_str(), \
            SPF_ASYNC | SPF_PURGEBEFORESPEAK, nullptr);
        if (SUCCEEDED(hr)) {
            log.info("Text spoken: " + std::string(text.begin(), text.end()));
        } else {
            log.error("Failed to speak text. HRESULT: " + std::to_string(hr));
        }
    } else {
        log.error("No SAPI voice instance available.");
    }

    // Деинициализируем COM для текущего потока
    CoUninitialize();
#else
    // Convert std::wstring to UTF-8
    std::wstring safeText = escapeForShell(text);
    std::string command = "espeak \"" + \
        std::string(safeText.begin(), safeText.end()) + "\"";
    int result = system(command.c_str());
    if (result == 0) {
        log.info("Text spoken (Linux): " + \
            std::string(text.begin(), text.end()));
    } else {
        log.error("Failed to run TTS command on Linux.");
    }
#endif
}

#ifndef _WIN32
// Escape quotes and special characters for shell usage
std::wstring TextToSpeech::escapeForShell(const std::wstring &input) {
    std::wstring escaped;
    for (wchar_t ch : input) {
        if (ch == L'"' || ch == L'\\') {
            escaped += L'\\';
        }
        escaped += ch;
    }
    return escaped;
}
#endif
