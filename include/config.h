// config.h
#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
public:
    // Объявление статических переменных
    static std::string app_name;
    static std::string app_version;
    static std::string microphone;
    static std::string log_path;
    static std::string wav_path;
    static std::string output_txt_path;
    static int silence_db;
    static std::string whisper_cli_path;
    static std::string whisper_model_path;
    static std::string koboldcpp_link;
    static std::string koboldcpp_path;
    static std::string koboldcpp_cfg_path;
    static std::string koboldcpp_model_path;

    // Метод для загрузки конфигурации из файла
    static bool loadConfig(const std::string& filepath);

    // Метод для быстрого изменения параметра в конфиг-файле
    static bool updateConfig(const std::string& filepath,
        const std::string& parameter,
        const std::string& value);

private:
    // Вспомогательная функция для обработки строки конфигурации
    static void parseLine(const std::string& line);

    // Вспомогательная функция для сохранения конфига
    static bool saveConfig(const std::string& filepath,
        const std::map<std::string, std::string>& configMap);
};

#endif  // CONFIG_H
