#include "../include/recognizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

Recognizer::Recognizer(const std::string& whisper_path, const std::string& model_path, Log& log)
    : whisper_path(whisper_path), model_path(model_path), log(log) {
    log.info("Recognizer initialized with Whisper path: " + whisper_path + " and model path: " + model_path);
}

std::string Recognizer::recognize(const std::string& filename) {
    log.info("Starting recognition for file: " + filename);
    std::cout << std::endl << "Processing...\n";

    // Генерируем имя выходного текстового файла
    std::string output_txt_file = filename + ".txt";

    // Формируем команду для Whisper
    std::string command = whisper_path + " -m " + model_path +
        " -f " + filename +
        " --language en --output-txt" +
        " --no-prints";

    log.info("Executing command: " + command);

    // Запускаем команду
    int result = std::system(command.c_str());

    // Проверяем результат выполнения команды
    if (result != 0) {
        log.error("Recognition failed for file: " + filename);
        std::cerr << "Recognition failed for file: " + filename << std::endl;
        return "error";
    }

    // Читаем результат из текстового файла
    std::ifstream file(output_txt_file);
    if (!file) {
        log.error("Failed to open output file: " + output_txt_file);
        std::cerr << "Failed to open output file: " + output_txt_file << std::endl;
        return "error";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string recognized_text = buffer.str();

    log.info("Recognition completed for file: " + filename);
    return recognized_text;
}