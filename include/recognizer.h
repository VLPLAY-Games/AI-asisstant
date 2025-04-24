  //  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <string>

class Recognizer {
 public:
    Recognizer(const std::string& whisperPath, const std::string& modelPath);
    std::string recognize(const std::string& filename);

 private:
    std::string whisperPath;
    std::string modelPath;
};

#endif
