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
    log.info("===================   " + Config::app_name + " v" + Config::app_version + "   ===================");
    std::cout << Config::app_name + " v" + Config::app_version << std::endl;
    log.info("Program started");

    log.info("Initializing recorder...");
    Recorder recorder(Config::microphone, log);

    log.info("Initializing recognizer...");
    Recognizer recognizer(Config::whisper_cli_path, Config::whisper_model_path, log);

    log.info("Initializing text-to-speech...");
    TextToSpeech tts(log);

    log.info("Initializing koboldcpp...");
    KoboldClient kobold(Config::koboldcpp_link, log);

    while (true) {
        recorder.record(Config::wav_path, Config::silence_db);
        std::string speech = recognizer.recognize(Config::wav_path);
        if (speech != "error") {
            std::string response = kobold.sendRequest(speech);

            std::wstring w_response(response.begin(), response.end());
            tts.speak(w_response);

            std::transform(speech.begin(), speech.end(), speech.begin(), ::tolower);
            if (speech.find("stop") != std::string::npos || speech.find("exit") != std::string::npos) {
                break;
            }
        }
        else {
            log.error("An error has occurred while performing main task");
        }
    }

    log.info("Program closed");
    log.info("=========================================================");
    log.close_log();
    return 0;
}