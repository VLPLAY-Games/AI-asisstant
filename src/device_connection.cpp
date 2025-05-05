// Copyright MIT License 2025 VL_PLAY Games


#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "../include/device_connection.h"
#include "../include/log.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <optional>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket close
#endif

DC::DC(Log& log) : log(log) {
#ifdef _WIN32
    initWinsock();
#endif
    log.info("DC instance created.");
}

DC::~DC() {
#ifdef _WIN32
    WSACleanup();
#endif
    log.info("DC instance destroyed.");
}

#ifdef _WIN32
void DC::initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        log.error("WSAStartup failed.");
    }
    else {
        log.info("Winsock initialized.");
    }
}
#endif

void DC::scanLocalNetwork(const std::string& base_ip, int port, int threads) {
    log.info("Starting local network scan on base IP: " + base_ip + " with " + std::to_string(threads) + " threads.");

    int range = 255 / threads;
    std::vector<std::thread> threadList;
    for (int i = 0; i < threads; ++i) {
        int start = i * range + 1;
        int end = (i == threads - 1) ? 254 : (start + range - 1);
        threadList.emplace_back(&DC::scanRange, this, base_ip, start, end, port);
        log.info("Thread " + std::to_string(i) + " scanning IPs: " + base_ip + std::to_string(start) + " to " + base_ip + std::to_string(end));
    }

    for (auto& t : threadList) {
        if (t.joinable()) t.join();
    }

    log.info("Finished network scan.");
}

void DC::scanRange(const std::string& base_ip, int start, int end, int port) {
    for (int i = start; i <= end; ++i) {
        std::ostringstream ipStream;
        ipStream << base_ip << i;
        std::string ip = ipStream.str();

        if (isPortOpen(ip, port)) {
            std::lock_guard<std::mutex> lock(deviceMutex);
            discoveredDevices.push_back(ip);
            log.info("Discovered device at " + ip + ":" + std::to_string(port));
        }
    }
}

bool DC::isPortOpen(const std::string& ip, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        log.error("Socket creation failed for IP: " + ip);
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // Set timeout
#ifdef _WIN32
    DWORD timeout = 300;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 300000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

    bool result = connect(sock, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR;
    if (result) {
        log.info("Port " + std::to_string(port) + " is open on " + ip);
    }
    else {
        log.warning("Port " + std::to_string(port) + " is closed on " + ip);
    }

    closesocket(sock);
    return result;
}

bool DC::sendCommand(const std::string& ip, const std::string& command, int port) {
    log.info("Sending command to " + ip + ":" + std::to_string(port) + " -> " + command);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        log.error("Socket creation failed");
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        log.error("Connection failed to " + ip + ":" + std::to_string(port));
        closesocket(sock);
        return false;
    }

    send(sock, command.c_str(), static_cast<int>(command.size()), 0);

    char buffer[1024]{};
    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        log.info("Response from " + ip + ": " + std::string(buffer));
    }
    else {
        log.warning("No response from " + ip);
    }

    closesocket(sock);
    return true;
}

std::vector<std::string> DC::getDiscoveredDevices() {
    std::lock_guard<std::mutex> lock(deviceMutex);
    log.info("Returning list of " + std::to_string(discoveredDevices.size()) + " discovered devices.");
    return discoveredDevices;
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
