// Copyright MIT License 2025 VL_PLAY Games

#pragma once
#ifndef DEVICE_CONNECTION_H
#define DEVICE_CONNECTION_H

#include "log.h"
#include <string>
#include <vector>
#include <mutex>
#include <optional>

class DC {
 public:
    explicit DC(Log& log);
    ~DC();

    void scanLocalNetwork(const std::string& base_ip, \
        int port, int threads = 10);
    std::vector<std::string> getDiscoveredDevices();
    std::string sendCommand(int deviceIndex, \
        const std::string& command, int port);

 private:
    Log& log;
    bool isPortOpen(const std::string& ip, int port);
    void scanRange(const std::string& base_ip, int start, int end, int port);
    std::string getLocalIPAddress();
    std::string getResponseFromDevice(const std::string& target_ip, \
        int port, const std::string& command, const std::string& sender_ip);


    std::vector<std::string> discoveredDevices;
    std::mutex deviceMutex;

#ifdef _WIN32
    void initWinsock();
#endif
};
#endif
