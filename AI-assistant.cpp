#include "include/recorder.h"
#include "include/recognizer.h"
#include "include/tts.h"
#include "include/kobold_client.h"
#include <algorithm>
#include <cctype>

int main() {
    Recorder recorder;
    Recognizer recognizer("software\\whisper.cpp\\bin\\whisper-cli.exe", "software\\whisper.cpp\\models\\ggml-base.en.bin");

    TextToSpeech tts;

    KoboldClient kobold("http://localhost:5001/api/generate"); // Адрес сервера KoboldCpp
    
    while (true) {
        recorder.record("voice_record.wav");
        std::string speech = recognizer.recognize("voice_record.wav");

        std::string response = kobold.sendRequest(speech);

        std::wstring w_responce(response.begin(), response.end());
        tts.speak(w_responce);

        /*std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        if (text.find("stop") != std::string::npos || text.find("exit") != std::string::npos) {
            break;
        }*/
    }

    return 0;
}