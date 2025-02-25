#pragma once

#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>

class Log {
public:
    Log(const std::string& log_path);
    ~Log();
    void debug(const std::string& text);
    void info(const std::string& text);
    void warning(const std::string& text);
    void error(const std::string& text);
    void critical(const std::string& text);

private:
    std::string log_path;
    FILE* log_file;
};

#endif // LOG_H