#include "../include/tts.h"

TextToSpeech::TextToSpeech(Log& log)
    : initialized(false), log(log) {
    // Инициализация COM библиотеки
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        log.error("Failed to initialize COM library.");
        initialized = false;
    }
    else {
        initialized = true;
        log.info("TextToSpeech initialized successfully.");
    }
}

TextToSpeech::~TextToSpeech() {
    // Деинициализация COM библиотеки
    CoUninitialize();
    log.info("TextToSpeech destroyed.");
}

void TextToSpeech::speak(const std::wstring& text) {
    if (!initialized) {
        log.error("TextToSpeech is not initialized. Cannot speak.");
        return;
    }

    // Создание экземпляра голоса
    ISpVoice* pVoice = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    if (SUCCEEDED(hr)) {
        // Установка вывода и воспроизведение текста
        pVoice->SetOutput(nullptr, TRUE);
        pVoice->Speak(text.c_str(), SPF_IS_XML, nullptr);
        pVoice->Release();
        log.info("Text spoken: " + std::string(text.begin(), text.end()));
    }
    else {
        log.error("Failed to create SAPI voice instance.");
    }
}