#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    // Объявление статических переменных
    static std::string microphone;
    static std::string log_path;
    static std::string wav_path;
    static std::string output_txt_path;
    static int silence_db;
    static std::string whisper_cli_path;
    static std::string whisper_model_path;
    static std::string koboldcpp_link;

    // Метод для загрузки конфигурации из файла
    static bool loadConfig(const std::string& filepath);

private:
    // Вспомогательная функция для обработки строки конфигурации
    static void parseLine(const std::string& line);
};

#endif // CONFIG_H