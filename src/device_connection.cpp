// Copyright MIT License 2025 VL_PLAY Games

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "../include/device_connection.h"
#include "../include/log.h"
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket close
#endif

DC::DC(Log &log)
    : log(log) {
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
    } else {
        log.info("Winsock initialized.");
    }
}
#endif

void DC::scanLocalNetwork(const std::string &base_ip, int port, int threads) {
    log.info("Starting local network scan on base IP: " + base_ip + " with "
             + std::to_string(threads) + " threads.");

    int range = 255 / threads;
    std::vector<std::thread> threadList;
    for (int i = 0; i < threads; ++i) {
        int start = i * range + 1;
        int end = (i == threads - 1) ? 254 : (start + range - 1);
        threadList.emplace_back(&DC::scanRange, this, \
                                base_ip, start, end, port);
        log.info("Thread " + std::to_string(i) + " scanning IPs: " + \
                 base_ip + std::to_string(start)
                 + " to " + base_ip + std::to_string(end));
    }

    for (auto &t : threadList) {
        if (t.joinable())
            t.join();
    }

    log.info("Finished network scan.");
}

void DC::scanRange(const std::string &base_ip, int start, int end, int port) {
    for (int i = start; i <= end; ++i) {
        std::ostringstream ipStream;
        ipStream << base_ip << i;
        std::string ip = ipStream.str();

        if (isPortOpen(ip, port)) {
            // Отправка команды "Connected requested" и ожидание ответа
            log.info("Sending connection request to " + \
                ip + ":" + std::to_string(port));
            std::string sender_ip = getLocalIPAddress();
            std::string response = getResponseFromDevice(ip, \
                port, "Connected requested", sender_ip);

            // Если ответ "Connection accepted", добавляем устройство в массив
            if (response == "SUCCESS: Connection accepted") {
                std::lock_guard<std::mutex> lock(deviceMutex);
                discoveredDevices.push_back(ip);
                log.info("Discovered and accepted device at " + \
                    ip + ":" + std::to_string(port));
            } else {
                log.warning("Connection to " + ip + " declined or failed.");
            }
        }
    }
}

bool DC::isPortOpen(const std::string &ip, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        log.error("Socket creation failed for IP: " + ip);
        return false;
    }

    // Установка неблокирующего режима
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    fcntl(sock, F_SETFL, O_NONBLOCK);
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    connect(sock, (sockaddr *) &addr, sizeof(addr));

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 30000;  // 30 мс

    bool isConnected = false;

    int sel = select(static_cast<int>(sock + 1), \
                nullptr, &writefds, nullptr, &timeout);
    if (sel > 0 && FD_ISSET(sock, &writefds)) {
        int err = 0;
#ifdef _WIN32
        int len = sizeof(err);
#else
        socklen_t len = sizeof(err);
#endif
        getsockopt(sock, SOL_SOCKET, SO_ERROR, \
            reinterpret_cast<char *>(&err), &len);
        if (err == 0) {
            isConnected = true;
            log.info("Port " + std::to_string(port) + " is open on " + ip);
        }
    }

    closesocket(sock);

    return isConnected;
}

std::string DC::sendCommand(int deviceIndex, \
    const std::string &command, int port) {
    std::lock_guard<std::mutex> lock(deviceMutex);
    if (discoveredDevices.empty()) {
        log.error("No discovered devices available.");
        return "0";
    }

    if (deviceIndex < 0 || \
        deviceIndex >= static_cast<int>(discoveredDevices.size())) {
        log.error("Invalid device index: " + std::to_string(deviceIndex));
        return "0";
    }

    const std::string &target_ip = discoveredDevices[deviceIndex];
    std::string sender_ip = getLocalIPAddress();

    log.info("Sending command to device #" + \
        std::to_string(deviceIndex + 1) + " at " + target_ip
             + ":" + std::to_string(port) + " -> " + command);

    // Получаем ответ от устройства
    std::string response = \
        getResponseFromDevice(target_ip, port, command, sender_ip);
    if (!response.empty()) {
        // Дополнительная логика, если ответ получен
        log.info("Command executed successfully. Response: " + response);
        return response;
    }

    // Если ответ пустой или ошибка
    log.error("Command execution failed.");
    return "0";
}

std::string DC::getLocalIPAddress() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        return "unknown";
    }

    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return "unknown";
    }

    struct in_addr *addr = reinterpret_cast<struct in_addr *>(host->h_addr);
    return inet_ntoa(*addr);
}

std::string DC::getResponseFromDevice(const std::string &target_ip,
                                      int port,
                                      const std::string &command,
                                      const std::string &sender_ip) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        log.error("Socket creation failed");
        return "";
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(target_ip.c_str());

    if (connect(sock, (sockaddr *) &addr, \
        sizeof(addr)) == SOCKET_ERROR) {
        log.error("Connection failed to " + \
            target_ip + ":" + std::to_string(port));
        closesocket(sock);
        return "";
    }

    std::string formattedCommand = "FROM: " + sender_ip + " | " + command;
    send(sock, formattedCommand.c_str(), \
        static_cast<int>(formattedCommand.size()), 0);

    char buffer[1024]{};
    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        log.info("Response from " + target_ip + ": " + std::string(buffer));
        closesocket(sock);
        return std::string(buffer);
    } else {
        log.warning("No response from " + target_ip);
        closesocket(sock);
        return "";
    }
}

std::vector<std::string> DC::getDiscoveredDevices() {
    std::lock_guard<std::mutex> lock(deviceMutex);
    log.info("Returning list of " + std::to_string(discoveredDevices.size())
             + " discovered devices.");
    return discoveredDevices;
}
