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

    void scanLocalNetwork(const std::string& base_ip, int port, int threads = 10);
    std::vector<std::string> getDiscoveredDevices();
    bool sendCommand(const std::string& ip, const std::string& command, int port);

private:
    Log& log;
    bool isPortOpen(const std::string& ip, int port);
    void scanRange(const std::string& base_ip, int start, int end, int port);


    std::vector<std::string> discoveredDevices;
    std::mutex deviceMutex;

#ifdef _WIN32
    void initWinsock();
#endif
};
#endif
