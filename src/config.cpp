// config.cpp
#include "../include/config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

// Определение статических переменных
std::string Config::app_name = "";
std::string Config::app_version = "";
std::string Config::microphone = "";
std::string Config::log_path = "";
std::string Config::wav_path = "";
std::string Config::output_txt_path = "";
int Config::silence_db = 0;
std::string Config::whisper_cli_path = "";
std::string Config::whisper_model_path = "";
std::string Config::koboldcpp_link = "";
std::string Config::koboldcpp_path = "";
std::string Config::koboldcpp_cfg_path = "";
std::string Config::koboldcpp_model_path = "";

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

        parseLine(line);
    }

    configFile.close();
    return true;
}

bool Config::updateConfig(const std::string& filepath,
    const std::string& parameter,
    const std::string& value) {
    // Читаем весь конфиг в map
    std::map<std::string, std::string> configMap;
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

        std::istringstream iss(line);
        std::string key, val;
        if (std::getline(iss, key, '=') && std::getline(iss, val)) {
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);
            configMap[key] = val;
        }
    }
    configFile.close();

    // Обновляем нужный параметр
    configMap[parameter] = value;

    // Сохраняем обратно
    return saveConfig(filepath, configMap);
}

bool Config::saveConfig(const std::string& filepath,
    const std::map<std::string, std::string>& configMap) {
    std::ofstream configFile(filepath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Cannot open config file for writing " << filepath << std::endl;
        return false;
    }

    // Записываем все параметры
    for (const auto& [key, value] : configMap) {
        configFile << key << " = " << value << "\n";
    }

    configFile.close();
    return true;
}

void Config::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;
    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (key == "app_name") app_name = value;
        else if (key == "app_version") app_version = value;
        else if (key == "microphone") microphone = value;
        else if (key == "log_path") log_path = value;
        else if (key == "wav_path") wav_path = value;
        else if (key == "output_txt_path") output_txt_path = value;
        else if (key == "silence_db") silence_db = std::stoi(value);
        else if (key == "whisper_cli_path") whisper_cli_path = value;
        else if (key == "whisper_model_path") whisper_model_path = value;
        else if (key == "koboldcpp_link") koboldcpp_link = value;

        else if (key == "koboldcpp_path") koboldcpp_path = value;
        else if (key == "koboldcpp_cfg_path") koboldcpp_cfg_path = value;
        else if (key == "koboldcpp_model_path") koboldcpp_model_path = value;
        else std::cerr << "Warning: Unknown config key: " << key << std::endl;
    }
}
