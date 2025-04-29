  //  Copyright MIT License 2025 VL_PLAY Games


#define _CRT_SECURE_NO_DEPRECATE

#include "../include/log.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>

Log::Log(const std::string& log_path)
    : log_path(log_path) {
    log_file = fopen(log_path.c_str(), "a+");
    fclose(log_file);
}


void Log::close_log() {
    fclose(log_file);
}

Log::~Log() {
    close_log();
}

std::string Log::getCurrentTime() {
    // Получаем текущее время
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);

    // Форматируем время в строку
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Log::debug(const std::string& text) {
    std::cout << "[" + getCurrentTime() + "] [DEBUG] " + text + "\n";
    log_file = fopen(log_path.c_str(), "a+");
    std::string logEntry = "[" + getCurrentTime() + "] [DEBUG] " + text + "\n";
    fputs(logEntry.c_str(), log_file);
    fclose(log_file);
}

void Log::info(const std::string& text) {
    std::cout << "[" + getCurrentTime() + "] [INFO] " + text + "\n";
    log_file = fopen(log_path.c_str(), "a+");
    std::string logEntry = "[" + getCurrentTime() + "] [INFO] " + text + "\n";
    fputs(logEntry.c_str(), log_file);
    fclose(log_file);
}

void Log::warning(const std::string& text) {
    std::cout << "[" + getCurrentTime() + "] [WARNING] " + \
        text + "\n";
    log_file = fopen(log_path.c_str(), "a+");
    std::string logEntry = "[" + getCurrentTime() + "] [WARNING] " + \
        text + "\n";
    fputs(logEntry.c_str(), log_file);
    fclose(log_file);
}

void Log::error(const std::string& text) {
    std::cout << "[" + getCurrentTime() + "] [ERROR] " + text + "\n";
    log_file = fopen(log_path.c_str(), "a+");
    std::string logEntry = "[" + getCurrentTime() + "] [ERROR] " + text + "\n";
    fputs(logEntry.c_str(), log_file);
    fclose(log_file);
}

void Log::critical(const std::string& text) {
    std::cout << "[" + getCurrentTime() + "] [CRITICAL] " + \
        text + "\n";
    log_file = fopen(log_path.c_str(), "a+");
    std::string logEntry = "[" + getCurrentTime() + "] [CRITICAL] " + \
        text + "\n";
    fputs(logEntry.c_str(), log_file);
    fclose(log_file);
}

void Log::clear() {
    remove(log_path.c_str());
    log_file = fopen(log_path.c_str(), "a+");
    fclose(log_file);
}