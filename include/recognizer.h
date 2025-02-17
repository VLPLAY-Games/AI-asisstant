#pragma once
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <string>

class Recognizer {
public:
    Recognizer(const std::string& whisper_path, const std::string& model_path);
    std::string recognize(const std::string& filename);

private:
    std::string whisper_path;
    std::string model_path;
};

#endif // RECOGNIZER_H
