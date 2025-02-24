//#include "include/recorder.h"
//#include "include/recognizer.h"
//#include "include/tts.h"
//#include "include/kobold_client.h"
//#include <algorithm>
//#include <cctype>
//
//int main() {
//    Recorder recorder;
//    Recognizer recognizer("software\\whisper.cpp\\bin\\whisper-cli.exe", "software\\whisper.cpp\\models\\ggml-base.en.bin");
//
//    TextToSpeech tts;
//
//    KoboldClient kobold("http://localhost:5001/api/generate"); // Адрес сервера KoboldCpp
//    
//    while (true) {
//        recorder.record("voice_record.wav");
//        std::string speech = recognizer.recognize("voice_record.wav");
//
//        std::string response = kobold.sendRequest(speech);
//
//        std::wstring w_responce(response.begin(), response.end());
//        tts.speak(w_responce);
//
//        std::transform(speech.begin(), speech.end(), speech.begin(), ::tolower);
//        if (speech.find("stop") != std::string::npos || speech.find("exit") != std::string::npos) {
//            break;
//        }
//    }
//
//    return 0;
//}

#include "include/recorder.h"
#include "include/recognizer.h"
#include "include/tts.h"
#include "include/kobold_client.h"
#include <algorithm>
#include <cctype>
#include <iostream>

int main() {
    Recorder recorder;
    Recognizer recognizer("software\\whisper.cpp\\bin\\whisper-cli.exe", "software\\whisper.cpp\\models\\ggml-base.en.bin");

    TextToSpeech tts;

    KoboldClient kobold("http://localhost:5001/api/generate");

    recorder.setOnSpeechStartCallback([]() {
        std::cout << "Speech detected. Recording started...\n";
        });

    recorder.setOnSpeechEndCallback([]() {
        std::cout << "Speech ended. Processing...\n";
        });

    while (true) {
        recorder.startRecording("voice_record.wav");

        while (recorder.isRecording()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::string speech = recognizer.recognize("voice_record.wav");
        std::cout << "You said: " << speech << "\n";

        std::transform(speech.begin(), speech.end(), speech.begin(), ::tolower);
        if (speech.find("stop") != std::string::npos || speech.find("exit") != std::string::npos) {
            std::cout << "Exiting...\n";
            break;
        }

        std::string response = kobold.sendRequest(speech);
        std::cout << "AI response: " << response << "\n";

        std::wstring w_response(response.begin(), response.end());
        tts.speak(w_response);
    }

    return 0;
}