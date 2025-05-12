//  Copyright MIT License 2025 VL_PLAY Games

#include "../include/tts.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

TextToSpeech::TextToSpeech(Log &log)
    : initialized(false)
    , log(log)
{
    std::cout << "\n###############################\n";
    std::cout << "     Initializing TTS          \n";
    std::cout << "###############################\n\n";

#ifdef _WIN32
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        log.error("Failed to initialize COM library.");
    } else {
        initialized = true;
        log.info("TextToSpeech initialized successfully (Windows).");
    }
#else
    initialized = true;
    log.info("TextToSpeech initialized successfully (Linux).");
#endif

    std::cout << "\n############################\n";
    std::cout << "        TTS Ready           \n";
    std::cout << "############################\n\n";
}

TextToSpeech::~TextToSpeech()
{
#ifdef _WIN32
    CoUninitialize();
#endif
    log.info("TextToSpeech destroyed.");
}

void TextToSpeech::speak(const std::wstring &text)
{
    std::wcout << std::endl;
    if (!initialized) {
        log.error("TextToSpeech is not initialized. Cannot speak.");
        return;
    }

#ifdef _WIN32
    ISpVoice *pVoice = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SpVoice,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_ISpVoice,
                                  (void **) &pVoice);
    if (SUCCEEDED(hr)) {
        pVoice->SetOutput(nullptr, TRUE);
        pVoice->Speak(text.c_str(), SPF_IS_XML, nullptr);
        pVoice->Release();
        log.info("Text spoken: " + std::string(text.begin(), text.end()));
    } else {
        log.error("Failed to create SAPI voice instance.");
    }
#else
    // Convert std::wstring to UTF-8
    std::wstring safeText = escapeForShell(text);
    std::string command = "espeak \"" + std::string(safeText.begin(), safeText.end()) + "\"";
    int result = system(command.c_str());
    if (result == 0) {
        log.info("Text spoken (Linux): " + std::string(text.begin(), text.end()));
    } else {
        log.error("Failed to run TTS command on Linux.");
    }
#endif
}

#ifndef _WIN32
// Escape quotes and special characters for shell usage
std::wstring TextToSpeech::escapeForShell(const std::wstring &input)
{
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

// sudo apt install espeak
