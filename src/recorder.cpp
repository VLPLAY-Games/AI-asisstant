#include "../include/recorder.h"
#include <iostream>
#include <cstdlib>

Recorder::Recorder(const std::string& microphone_name)
    : microphone_name(microphone_name) {
}

void Recorder::record(const std::string& filename, int duration) {
    std::cout << "Listening...\n";

    std::string command = "ffmpeg -f dshow -i audio=\"" + microphone_name +
        "\" -t " + std::to_string(duration) +
        " -ac 1 -ar 16000 " + filename + " -y -loglevel error";

    std::system(command.c_str());
    std::cout << "Done listening.\n";
}
