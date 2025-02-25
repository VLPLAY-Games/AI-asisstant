#include "include/config.h"
#include "include/log.h"
#include "include/recorder.h"
#include "include/recognizer.h"
#include "include/tts.h"
#include "include/kobold_client.h"
#include <algorithm>
#include <cctype>

int main() {
    Config::loadConfig("files/config.cfg");
    Log log(Config::log_path);

    log.info("Programm started");
    Recorder recorder(Config::microphone);
    Recognizer recognizer(Config::whisper_cli_path, Config::whisper_model_path);
    TextToSpeech tts;
    KoboldClient kobold(Config::koboldcpp_link);
    
    while (true) {
        recorder.record(Config::wav_path);
        std::string speech = recognizer.recognize(Config::wav_path);
        std::cout << speech << std::endl;



        std::string response = kobold.sendRequest(speech);

        std::wstring w_responce(response.begin(), response.end());
        tts.speak(w_responce);

        /*std::transform(speech.begin(), speech.end(), speech.begin(), ::tolower);
        if (speech.find("stop") != std::string::npos || speech.find("exit") != std::string::npos) {
            break;
        }*/
    }

    return 0;
}
