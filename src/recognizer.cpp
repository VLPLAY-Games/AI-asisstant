  //  Copyright MIT License 2025 VL_PLAY Games


#include "../include/recognizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>

Recognizer::Recognizer(const std::string& whisperPath, \
    const std::string& modelPath)
    : whisperPath(whisperPath), modelPath(modelPath) {
}

std::string Recognizer::recognize(const std::string& filename) {
    remove((filename + ".txt").c_str());

    std::ofstream temp;
    temp.open(filename + ".txt");
    temp.close();

    std::string command = whisperPath + " -m \"" + modelPath + "\" -f \"" +
        filename + "\" --language en --output-txt --no-prints " +
        "--suppress-nst";
    std::system(command.c_str());

    std::ifstream file(filename + ".txt");
    if (!file) {
        std::cerr << "Failed to open result file.\n";
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
