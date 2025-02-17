#include "../include/tts.h"

TextToSpeech::TextToSpeech() {
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        std::cerr << "Failed to initialize COM library." << std::endl;
        initialized = false;
    }
    else {
        initialized = true;
    }
}

TextToSpeech::~TextToSpeech() {
    CoUninitialize();
}

void TextToSpeech::speak(const std::wstring& text) {
    if (!initialized) return;

    ISpVoice* pVoice = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    if (SUCCEEDED(hr)) {
        pVoice->SetOutput(nullptr, TRUE);
        pVoice->Speak(text.c_str(), SPF_IS_XML, nullptr);
        pVoice->Release();
    }
    else {
        std::cerr << "Failed to create SAPI voice instance." << std::endl;
    }
}
