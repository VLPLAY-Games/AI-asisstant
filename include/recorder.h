//#pragma once
//
//#ifndef RECORDER_H
//#define RECORDER_H
//#include <string>
//
//class Recorder {
//public:
//    Recorder(const std::string& microphone_name = "Microphone (G435 Wireless Gaming Headset)");
//    void record(const std::string& filename, int duration = 5);
//
//private:
//    std::string microphone_name;
//};
//
//#endif // RECORDER_H


#pragma once
#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <functional>
#include <atomic>
#include <thread>

class Recorder {
public:
    Recorder(const std::string& microphone_name = "Microphone (G435 Wireless Gaming Headset)");
    ~Recorder();

    void startRecording(const std::string& filename);
    void stopRecording();

    bool isRecording() const;

    void setOnSpeechStartCallback(const std::function<void()>& callback);
    void setOnSpeechEndCallback(const std::function<void()>& callback);

private:
    std::string microphone_name;
    std::atomic<bool> is_recording;
    std::function<void()> on_speech_start;
    std::function<void()> on_speech_end;

    void processAudioStream(const std::string& filename);
};

#endif // RECORDER_H