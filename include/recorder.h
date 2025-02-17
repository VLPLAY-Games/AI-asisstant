#pragma once

#ifndef RECORDER_H
#define RECORDER_H

#include <string>

class Recorder {
public:
    Recorder(const std::string& microphone_name = "Microphone (2- USB PnP Audio Device)");
    void record(const std::string& filename, int duration = 15);

private:
    std::string microphone_name;
};

#endif // RECORDER_H
