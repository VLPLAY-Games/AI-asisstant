// Copyright MIT License 2025 VL_PLAY Games

#pragma once
#ifndef DEVICE_CONNECTION_H
#define DEVICE_CONNECTION_H

#include <string>
#include <vector>
#include <mutex>

class DC {
public:
    DC();
    ~DC();

    void scanLocalNetwork(const std::string& base_ip, int port, int threads = 10);
    std::vector<std::string> getDiscoveredDevices();
    bool sendCommand(const std::string& ip, const std::string& command, int port);

private:
    bool isPortOpen(const std::string& ip, int port);
    void scanRange(const std::string& base_ip, int start, int end, int port);

    std::vector<std::string> discoveredDevices;
    std::mutex deviceMutex;
};

#endif
