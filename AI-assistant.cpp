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

    while (true) {
        recorder.record("voice_record.wav");
        std::string text = recognizer.recognize("voice_record.wav");
        std::wstring wtext(text.begin(), text.end());
        tts.speak(wtext);

        /*std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        if (text.find("stop") != std::string::npos || text.find("exit") != std::string::npos) {
            break;
        }*/
    }

    return 0;
}