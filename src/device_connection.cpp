// Copyright MIT License 2025 VL_PLAY Games

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "../include/device_connection.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

std::mutex cout_mutex;

DC::DC() {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error with WSAStartup\n";
    }
}

DC::~DC() {
    WSACleanup();
}

bool DC::isPortOpen(const std::string& ip, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    timeval timeout{};
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    int result = connect(sock, (sockaddr*)&addr, sizeof(addr));
    closesocket(sock);

    return result == 0;
}

void DC::scanRange(const std::string& base_ip, int start, int end, int port) {
    for (int i = start; i <= end; ++i) {
        std::string ip = base_ip + std::to_string(i);
        if (isPortOpen(ip, port)) {
            {
                std::lock_guard<std::mutex> lock(deviceMutex);
                discoveredDevices.push_back(ip);
            }
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Open port " << port << " on " << ip << std::endl;
        }
    }
}

void DC::scanLocalNetwork(const std::string& base_ip, int port, int threads) {
    discoveredDevices.clear();

    std::vector<std::thread> threadPool;
    int range = 254;
    int step = range / threads;

    for (int i = 0; i < threads; ++i) {
        int start = i * step + 1;
        int end = (i == threads - 1) ? 254 : (i + 1) * step;
        threadPool.emplace_back(&DC::scanRange, this, base_ip, start, end, port);
    }

    for (auto& t : threadPool) t.join();
}

std::vector<std::string> DC::getDiscoveredDevices() {
    std::lock_guard<std::mutex> lock(deviceMutex);
    return discoveredDevices;
}

bool DC::sendCommand(const std::string& ip, const std::string& command, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Cannot create Socket\n";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Cannot connect to " << ip << " on port " << port << std::endl;
        closesocket(sock);
        return false;
    }

    int sent = send(sock, command.c_str(), static_cast<int>(command.length()), 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "Error while sending command\n";
        closesocket(sock);
        return false;
    }

    std::cout << "Command send to " << ip << ": " << command << std::endl;
    // Приём ответа
    char buffer[1024]{};
    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "Received from " << ip << ": " << buffer << std::endl;
    }
    else {
        std::cout << "Answer from " << ip << " not received or empty.\n";
    }

    closesocket(sock);
    return true;
}


// Использование
//
// DC scanner;
// scanner.scanLocalNetwork("192.168.1.", 50505);
//
// auto devices = scanner.getDiscoveredDevices();
// std::cout << "\nFound devices:\n";
// for (const auto& ip : devices) {
//     std::cout << ip << std::endl;
// }
