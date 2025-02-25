#include "../include/recorder.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

Recorder::Recorder(const std::string& microphone_name, Log& log)
    : microphone_name(microphone_name), stopRecording(false), log(log) {
    log.info("Recorder initialized with microphone: " + microphone_name);
}

void Recorder::record(const std::string& filename, int silence_db) {
    log.info("Starting recording to file: " + filename);
    std::cout << "Listening...\n";

    // Формируем команду для ffmpeg
    std::string command = "ffmpeg -f dshow -i audio=\"" + microphone_name +
        "\" -af \"silencedetect=n=" + std::to_string(silence_db) + "dB:d = 3\" -ar 16000 -ac 1 -c:a pcm_s16le " + filename +
        " -y -loglevel info 2>&1";

    // Запускаем процесс ffmpeg в отдельном потоке
    std::thread ffmpegThread([this, command]() {
        FILE* pipe = _popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Error opening pipe.\n";
            log.error("Failed to open pipe for ffmpeg command.");
            return;
        }

        // Читаем вывод из pipe
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string output(buffer);

            // Проверяем, если нашли строку с 'silence_start'
            if (output.find("silence_start") != std::string::npos) {
                log.info("Silence detected. Stopping the recording.");
                stopRecording = true;
                break; // Завершаем поток после обнаружения тишины
            }
        }
        });

    // Ожидаем завершения потока ffmpeg
    ffmpegThread.join();
    log.info("Recording finished and saved to: " + filename);
    std::cout << "Done listening.\n";
}