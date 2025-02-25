#define _CRT_SECURE_NO_DEPRECATE

#include "../include/log.h"
#include <fstream>

Log::Log(const std::string& log_path)
    : log_path(log_path) {
    log_file = fopen(log_path.c_str(), "a+");
}

Log::~Log() {
    fclose(log_file);
}

void Log::debug(const std::string& text) {
    fputs(("[DEBUG] " + text + "\n").c_str(), log_file);
}

void Log::info(const std::string& text) {
    fputs(("[INFO] " + text + "\n").c_str(), log_file);
}

void Log::warning(const std::string& text) {
    fputs(("[WARNING] " + text + "\n").c_str(), log_file);
}

void Log::error(const std::string& text) {
    fputs(("[ERROR] " + text + "\n").c_str(), log_file);
}


void Log::critical(const std::string& text) {
    fputs(("[CRITICAL] " + text + "\n").c_str(), log_file);
}