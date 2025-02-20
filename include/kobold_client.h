#pragma once
#ifndef KOBOLD_CLIENT_H
#define KOBOLD_CLIENT_H



#define CURL_STATICLIB

#include <string>
#include <iostream>
#include <curl/curl.h> // Используем cURL для HTTP-запросов
#include <nlohmann/json.hpp> // Подключение библиотеки для работы с JSON

using json = nlohmann::json;

class KoboldClient {
public:
    explicit KoboldClient(const std::string& server_url);
    ~KoboldClient();

    std::string escapeJsonString(const std::string& str);
    std::string getResponseFromJson(const std::string& jsonResponse);
    std::string sendRequest(const std::string& prompt);

private:
    std::string server_url;

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};

#endif // KOBOLD_CLIENT_H
