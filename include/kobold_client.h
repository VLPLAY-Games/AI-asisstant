﻿#pragma once
#ifndef KOBOLD_CLIENT_H
#define KOBOLD_CLIENT_H

#define CURL_STATICLIB

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "log.h"

using json = nlohmann::json;

class KoboldClient {
public:
    // Конструктор с параметром URL сервера и объектом Log
    explicit KoboldClient(const std::string& server_url, Log& log);

    // Деструктор
    ~KoboldClient();

    // Метод для экранирования строки JSON
    std::string escapeJsonString(const std::string& str);

    // Метод для извлечения ответа из JSON
    std::string getResponseFromJson(const std::string& jsonResponse);

    // Метод для отправки запроса к серверу KoboldCpp
    std::string sendRequest(const std::string& prompt);

private:
    std::string server_url; // URL сервера KoboldCpp
    Log& log;              // Ссылка на объект Log для записи в лог

    // Callback-функция для записи данных от cURL
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};

#endif // KOBOLD_CLIENT_H