#include "../include/recorder.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

Recorder::Recorder(const std::string& microphone_name)
    : microphone_name(microphone_name), stopRecording(false) {
}

void Recorder::record(const std::string& filename, int duration) {
    std::cout << "Listening...\n";

    // Формируем команду для ffmpeg
    std::string command = "ffmpeg -f dshow -i audio=\"" + microphone_name +
        "\" -af \"silencedetect=n=-20dB:d=3\" -ar 16000 -ac 1 -c:a pcm_s16le " + filename +
        " -y -loglevel info 2>&1";

    // Запускаем процесс ffmpeg в отдельном потоке
    std::thread ffmpegThread([this, command]() {
        FILE* pipe = _popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Error opening pipe.\n";
            return;
        }

        // Читаем вывод из pipe
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string output(buffer);

            // Проверяем, если нашли строку с 'silence_start'
            if (output.find("silence_start") != std::string::npos) {
                std::cout << "Silence detected. Stopping the recording...\n";
                stopRecording = true;
                break; // Завершаем поток после обнаружения тишины
            }
        }
        });

    // Ожидаем завершения потока ffmpeg
    ffmpegThread.join();
    std::cout << "Done listening.\n";
}
