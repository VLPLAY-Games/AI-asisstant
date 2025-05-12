// config.h
#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
public:
    // Статические переменные конфигурации
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

    static std::string dc_subnet;  // e.g. "192.168.0."
    static int dc_port;            // e.g. 50505

    // Методы конфигурации
    static bool loadConfig(const std::string& filepath);
    static bool updateConfig(const std::string& filepath,
        const std::string& parameter,
        const std::string& value);
    static bool saveAllConfig(const std::string& filepath);

private:
    static void parseLine(const std::string& line);
    static bool saveConfig(const std::string& filepath,
        const std::map<std::string, std::string>& configMap);
};

#endif  // CONFIG_H
