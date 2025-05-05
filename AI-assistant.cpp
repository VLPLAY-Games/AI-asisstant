  //  Copyright MIT License 2025 VL_PLAY Games


#include <cctype>
#include <locale>
#include <string>
#include <iostream>
#include <algorithm>
#include "include/config.h"
#include "include/log.h"
#include "include/recorder.h"
#include "include/recognizer.h"
#include "include/tts.h"
#include "include/kobold_client.h"
#include "include/device_connection.h"

int main() {

    // Загрузка конфигурации
    if (!Config::loadConfig("files/config.cfg")) {
        std::cerr << "Failed to load configuration. Exiting.\n";
        return 1;
    }
    // Логгер
    Log log(Config::log_path);
    log.info("===================   " + Config::app_name + " v" + \
        Config::app_version + "   ===================");
    std::cout << Config::app_name + " v" + Config::app_version << std::endl;
    log.info("Program started");

    // Инициализация компонентов
    log.info("Initializing recorder...");
    Recorder recorder;
    recorder.listAvailableMicrophones();
    recorder.setMicrophone(Config::microphone);

    log.info("Initializing recognizer...");
    Recognizer recognizer(Config::whisper_cli_path, Config::whisper_model_path);

    log.info("Initializing text-to-speech...");
    TextToSpeech tts(log);

    log.info("Initializing device connection...");
    DC scanner(log);

    log.info("Initializing koboldcpp...");
    // KoboldClient kobold(Config::koboldcpp_link, Config::koboldcpp_path, Config::koboldcpp_cfg_path, Config::koboldcpp_model_path, log);

    // Основной цикл
    while (true) {
        log.info("Recording...");
        if (!recorder.record(Config::wav_path, Config::silence_db)) {
            log.error("Failed to record audio");
            continue;
        }

        log.info("Recognizing...");
        std::string speech = recognizer.recognize(Config::wav_path);

        if (speech != "error" && !speech.empty()) {
            log.info("Speech recognized: " + speech);

            // Преобразуем в нижний регистр
            std::transform(speech.begin(), speech.end(), speech.begin(), ::tolower);
            speech.erase(0, 1);

            // Проверка условий завершения
            if (speech.find("stop") != std::string::npos || speech.find("exit") != std::string::npos) {
                log.info("Stop/Exit command detected. Exiting loop.");
                break;
            }

            // Обработка команды "send to"
            std::cout << speech;
            if (speech.starts_with("send to device")) {
                std::string remote_command = speech.substr(15);  // пропустить "send to device"
                std::string target_ip = "";        // Заданный IP

                log.info("Sending remote command to " + target_ip + ": " + remote_command);

                if (scanner.sendCommand(target_ip, remote_command, 50505)) {
                    log.info("Command sent successfully to " + target_ip);
                }
                else {
                    log.warning("Failed to send command to " + target_ip);
                }

                continue; // skip local response if it's a remote command
            }
            //else {
            //    // Стандартный путь общения через kobold и TTS
            //    std::string response = kobold.sendRequest(speech);
            //    std::wstring w_response(response.begin(), response.end());
            //    tts.speak(w_response);
            //}

            
        }
        else {
            log.error("Recognition error or empty result");
        }
    }


    log.info("Program closed");
    log.info("=========================================================");
    log.close_log();
    return 0;
}
