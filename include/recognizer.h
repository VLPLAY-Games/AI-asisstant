//  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "../include/log.h"

class Recognizer {
 public:
    Recognizer(const std::string& whisperPath, \
      const std::string& modelPath, Log &log);
    std::string recognize(const std::string& filename);

 private:
    std::string whisperPath;
    std::string modelPath;
    Log& log;
};

#endif
