//  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>

class Log {
 public:
    explicit Log(const std::string& log_path);
    void close_log();
    ~Log();
    void debug(const std::string& text);
    void info(const std::string& text);
    void warning(const std::string& text);
    void error(const std::string& text);
    void critical(const std::string& text);
    void clear();

 private:
    std::string log_path;
    FILE* log_file;

    // Вспомогательный метод для получения текущего времени в формате строки
    std::string getCurrentTime();
};

#endif  // LOG_H
