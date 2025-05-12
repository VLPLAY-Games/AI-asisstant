//  Copyright MIT License 2025 VL_PLAY Games

#include "../include/recognizer.h"
#include "../include/log.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

Recognizer::Recognizer(const std::string &whisperPath, const std::string &modelPath, Log &log)
    : whisperPath(whisperPath)
    , modelPath(modelPath)
    , log(log)
{
    std::cout << "\n###############################\n";
    std::cout << "     Initializing Whisper    ";
    std::cout << "\n###############################\n\n";

    log.info("Initializing Whisper with CLI path: " + whisperPath + " and model path: " + modelPath);

    std::cout << "\n#########################################\n";
    std::cout << "     Initialized Whisper Successfully    ";
    std::cout << "\n#########################################\n\n";
}

std::string Recognizer::recognize(const std::string &filename)
{
    log.info("Starting speech recognition for file: " + filename);

    // Удаляем старый текстовый файл, если он существует
    std::string txtFile = filename + ".txt";
    if (remove(txtFile.c_str()) == 0) {
        log.info("Removed old text file: " + txtFile);
    }

    // Создаём пустой текстовый файл
    std::ofstream temp;
    temp.open(txtFile);
    if (!temp) {
        log.error("Failed to create temporary text file: " + txtFile);
        return "";
    }
    temp.close();

    // Формируем команду для Whisper
    std::string command = whisperPath + " -m \"" + modelPath + "\" -f \"" + filename
                          + "\" --language en --output-txt --no-prints " + "--suppress-nst";
    log.info("Executing Whisper command: " + command);

    // Выполняем команду
    int result = std::system(command.c_str());
    if (result != 0) {
        log.error("Whisper command failed with exit code: " + std::to_string(result));
        return "";
    }

    // Читаем результат из текстового файла
    std::ifstream file(txtFile);
    if (!file) {
        log.error("Failed to open result file: " + txtFile);
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string recognizedText = oss.str();
    if (recognizedText.empty()) {
        log.warning("Recognition result is empty for file: " + filename);
    } else {
        log.info("Successfully recognized speech: " + recognizedText);
    }
    return recognizedText;
}
