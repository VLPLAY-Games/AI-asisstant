//  Copyright MIT License 2025 VL_PLAY Games

#include "../include/recognizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

Recognizer::Recognizer(const std::string& whisperPath, \
    const std::string& modelPath)
    : whisperPath(whisperPath), modelPath(modelPath) {
    std::cout << "\n###############################\n";
    std::cout << "     Initializing Whisper    ";
    std::cout << "\n###############################\n\n";

    std::cout << "\n#########################################\n";
    std::cout << "     Initialized Whisper Successfully    ";
    std::cout << "\n#########################################\n\n";
}

std::string Recognizer::recognize(const std::string& filename) {
    remove((filename + ".txt").c_str());

    std::ofstream temp;
    temp.open(filename + ".txt");
    temp.close();

    // Create the command string
    std::string command = whisperPath + " -m \"" + modelPath + "\" -f \"" +
        filename + "\" --language en --output-txt --no-prints " +
        "--suppress-nst";

    // Check platform and adjust command execution
#ifdef _WIN32
    // On Windows, we use the system call as is
    std::system(command.c_str());
#else
    // On Linux, we need to ensure the command is executable
    if (access(whisperPath.c_str(), X_OK) != 0) {
        std::cerr << "Whisper executable not found or not executable on Linux." << std::endl;
        return "";
    }
    std::system(command.c_str());
#endif

    std::ifstream file(filename + ".txt");
    if (!file) {
        std::cerr << "Failed to open result file.\n";
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
