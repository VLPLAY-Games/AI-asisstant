#include "../include/kobold_client.h"
#include <iostream>

KoboldClient::KoboldClient(const std::string& server_url, Log& log)
    : server_url(server_url), log(log) {
    curl_global_init(CURL_GLOBAL_ALL);
    log.info("KoboldClient initialized with server URL: " + server_url);
}

KoboldClient::~KoboldClient() {
    curl_global_cleanup();
    log.info("KoboldClient destroyed.");
}

size_t KoboldClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string KoboldClient::escapeJsonString(const std::string& str) {
    std::string escapedStr;
    for (char c : str) {
        if (c == '"') {
            escapedStr += "\\\""; // Экранирование кавычек
        }
        else if (c == '\\') {
            escapedStr += "\\\\"; // Экранирование обратного слэша
        }
        else if (c == '\n') {
            escapedStr += "\\n"; // Экранирование новой строки
        }
        else if (c == '\t') {
            escapedStr += "\\t"; // Экранирование табуляции
        }
        else {
            escapedStr += c; // Оставляем остальные символы
        }
    }
    return escapedStr;
}

std::string KoboldClient::getResponseFromJson(const std::string& jsonResponse) {
    try {
        // Парсим строку JSON в объект
        json parsedJson = json::parse(jsonResponse);

        // Извлекаем значение поля "response"
        if (parsedJson.contains("response")) {
            return parsedJson["response"];
        }
        else {
            log.error("'response' field not found in JSON.");
            std::cerr << "'response' field not found in JSON." << std::endl;
            return "Error: 'response' field not found in JSON";
        }
    }
    catch (const std::exception& e) {
        log.error("Error parsing JSON: " + std::string(e.what()));
        std::cerr << "Error parsing JSON: " + std::string(e.what()) << std::endl;
        return "Error parsing JSON: " + std::string(e.what());
    }
}

std::string KoboldClient::sendRequest(const std::string& prompt) {
    std::cout << std::endl;
    CURL* curl = curl_easy_init();
    if (!curl) {
        log.error("Failed to initialize cURL.");
        std::cerr << "Failed to initialize cURL." << std::endl;
        return "";
    }

    std::string response;
    std::string escapedPrompt = escapeJsonString(prompt);
    std::string jsonData = R"({"prompt": ")" + escapedPrompt + R"(", "max_length": 100})"; // JSON-запрос к KoboldCpp

    log.info("Sending request to AI: " + jsonData);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        log.error("cURL request failed: " + std::string(curl_easy_strerror(res)));
        std::cerr << "cURL request failed: " + std::string(curl_easy_strerror(res)) << std::endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    log.info("AI response: " + response);

    return getResponseFromJson(response);
}