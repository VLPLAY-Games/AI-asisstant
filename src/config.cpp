#include "../include/config.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Определение статических переменных
std::string Config::microphone = "";
std::string Config::log_path = "";
std::string Config::wav_path = "";
std::string Config::output_txt_path = "";
int Config::silence_db = 0;
std::string Config::whisper_cli_path = "";
std::string Config::whisper_model_path = "";
std::string Config::koboldcpp_link = "";

bool Config::loadConfig(const std::string& filepath) {
    std::ifstream configFile(filepath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Cannot open config file " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Обрабатываем строку
        parseLine(line);
    }

    configFile.close();
    return true;
}

void Config::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;
    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
        // Удаляем пробелы вокруг ключа и значения
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Присваиваем значения переменным
        if (key == "microphone") {
            microphone = value;
        }
        else if (key == "log_path") {
            log_path = value;
        }
        else if (key == "wav_path") {
            wav_path = value;
        }
        else if (key == "output_txt_path") {
            output_txt_path = value;
        }
        else if (key == "silence_db") {
            silence_db = std::stoi(value);
        }
        else if (key == "whisper_cli_path") {
            whisper_cli_path = value;
        }
        else if (key == "whisper_model_path") {
            whisper_model_path = value;
        }
        else if (key == "koboldcpp_link") {
            koboldcpp_link = value;
        }
        else {
            std::cerr << "Warning: Unknown config key: " << key << std::endl;
        }
    }
}