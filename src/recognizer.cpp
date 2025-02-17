#include "../include/recognizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

Recognizer::Recognizer(const std::string& whisper_path, const std::string& model_path)
    : whisper_path(whisper_path), model_path(model_path) {
}

std::string Recognizer::recognize(const std::string& filename) {
    std::cout << "Processing...\n";

    std::string output_txt_file = filename + ".txt";
    std::string command = whisper_path + " -m " + model_path +
        " -f " + filename +
        " --language en --output-txt" +
        " --no-prints";

    std::system(command.c_str());

    std::ifstream file(output_txt_file);
    if (!file) return "Recognition failed!";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
