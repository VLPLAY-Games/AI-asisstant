//#include <iostream>
//#include <string>
//#include <vector>
//#include <set>
//#include <algorithm>
//#include <cctype>
//#include <mutex>
//
//#ifdef _WIN32
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <windows.h>
//#pragma comment(lib, "ws2_32.lib")
//#else
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <cstring>
//#define INVALID_SOCKET (-1)
//#define SOCKET_ERROR (-1)
//typedef int SOCKET;
//#endif
//
//std::set<std::string> trustedIps;
//std::mutex trustMutex;
//
//bool isTrusted(const std::string& ip) {
//    std::lock_guard<std::mutex> lock(trustMutex);
//    return trustedIps.count(ip) > 0;
//}
//
//void addTrustedIp(const std::string& ip) {
//    std::lock_guard<std::mutex> lock(trustMutex);
//    trustedIps.insert(ip);
//}
//
//std::string trim(const std::string& str) {
//    size_t first = str.find_first_not_of(" \t\n\r.");
//    size_t last = str.find_last_not_of(" \t\n\r.");
//    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
//}
//
//bool executeCommand(const std::string& command, std::string& statusMessage, bool& delayExecution) {
//    std::cout << "[INFO] Executing command: \"" << command << "\"" << std::endl;
//    delayExecution = false;
//
//#ifdef _WIN32
//    if (command == "run explorer") {
//        std::cout << "[ACTION] Launching File Explorer..." << std::endl;
//        system("explorer");
//        statusMessage = "Explorer launched.";
//        return true;
//    }
//    else if (command == "open browser") {
//        std::cout << "[ACTION] Opening browser..." << std::endl;
//        system("start chrome");
//        statusMessage = "Browser opened.";
//        return true;
//    }
//    else if (command == "open notepad") {
//        std::cout << "[ACTION] Opening Notepad..." << std::endl;
//        system("start notepad");
//        statusMessage = "Notepad opened.";
//        return true;
//    }
//    else if (command == "open paint") {
//        std::cout << "[ACTION] Opening Paint..." << std::endl;
//        system("mspaint");
//        statusMessage = "Paint opened.";
//        return true;
//    }
//    else if (command == "shutdown system") {
//        std::cout << "[ACTION] Preparing to shutdown system..." << std::endl;
//        statusMessage = "System shutdown will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "restart system") {
//        std::cout << "[ACTION] Preparing to restart system..." << std::endl;
//        statusMessage = "System restart will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "status") {
//        std::cout << "[ACTION] Checking system status..." << std::endl;
//        statusMessage = "System is running normally on Windows.";
//        return true;
//    }
//    else if (command == "open terminal") {
//        std::cout << "[ACTION] Opening Command Prompt..." << std::endl;
//        system("start cmd");
//        statusMessage = "Command Prompt opened.";
//        return true;
//    }
//    else if (command == "open task manager") {
//        std::cout << "[ACTION] Opening Task Manager..." << std::endl;
//        system("taskmgr");
//        statusMessage = "Task Manager opened.";
//        return true;
//    }
//    else if (command == "open control panel") {
//        std::cout << "[ACTION] Opening Control Panel..." << std::endl;
//        system("control");
//        statusMessage = "Control Panel opened.";
//        return true;
//    }
//#else
//    if (command == "run explorer") {
//        std::cout << "[ACTION] Launching file manager..." << std::endl;
//        system("xdg-open .");
//        statusMessage = "File manager launched.";
//        return true;
//    }
//    else if (command == "open browser") {
//        std::cout << "[ACTION] Opening browser..." << std::endl;
//        system("xdg-open http://");
//        statusMessage = "Browser opened.";
//        return true;
//    }
//    else if (command == "open notepad") {
//        std::cout << "[ACTION] Opening text editor (nano)..." << std::endl;
//        system("nano");
//        statusMessage = "Text editor (nano) opened.";
//        return true;
//    }
//    else if (command == "open paint") {
//        std::cout << "[ACTION] Opening paint-like application..." << std::endl;
//        system("gimp");
//        statusMessage = "Paint-like application opened.";
//        return true;
//    }
//    else if (command == "shutdown system") {
//        std::cout << "[ACTION] Preparing to shutdown system..." << std::endl;
//        statusMessage = "System shutdown will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "restart system") {
//        std::cout << "[ACTION] Preparing to restart system..." << std::endl;
//        statusMessage = "System restart will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "status") {
//        std::cout << "[ACTION] Checking system status..." << std::endl;
//        statusMessage = "System is running normally on Linux.";
//        return true;
//    }
//    else if (command == "open terminal") {
//        std::cout << "[ACTION] Opening terminal..." << std::endl;
//        system("gnome-terminal");
//        statusMessage = "Terminal opened.";
//        return true;
//    }
//    else if (command == "open task manager") {
//        std::cout << "[ACTION] Opening system monitor..." << std::endl;
//        system("gnome-system-monitor");
//        statusMessage = "Task Manager (system monitor) opened.";
//        return true;
//    }
//    else if (command == "open control panel") {
//        std::cout << "[WARNING] Control Panel is not available on Linux." << std::endl;
//        statusMessage = "Control Panel is a Windows-specific feature.";
//        return false;
//    }
//#endif
//
//    std::cout << "[ERROR] Unknown command: \"" << command << "\"" << std::endl;
//    statusMessage = "Unknown command.";
//    return false;
//}
//
//int main() {
//    std::cout << "AI Assistant Device Remote Server v1.2.0\n\n";
//
//#ifdef _WIN32
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "WSAStartup failed.\n";
//        return 1;
//    }
//#endif
//
//    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
//    if (listenSock == INVALID_SOCKET) {
//        std::cerr << "Socket creation failed.\n";
//        return 1;
//    }
//
//    sockaddr_in addr{};
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = INADDR_ANY;
//    addr.sin_port = htons(50505);
//
//    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
//        std::cerr << "Bind failed.\n";
//#ifdef _WIN32
//        closesocket(listenSock);
//        WSACleanup();
//#else
//        close(listenSock);
//#endif
//        return 1;
//    }
//
//    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
//        std::cerr << "Listen failed.\n";
//#ifdef _WIN32
//        closesocket(listenSock);
//        WSACleanup();
//#else
//        close(listenSock);
//#endif
//        return 1;
//    }
//
//    std::cout << "[READY] Listening on port 50505...\n";
//
//    while (true) {
//        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
//        if (clientSock == INVALID_SOCKET) {
//            std::cerr << "[ERROR] Accept failed.\n";
//            continue;
//        }
//
//        char buffer[1024]{};
//        int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//        if (received > 0) {
//            buffer[received] = '\0';
//            std::string input(buffer);
//
//            size_t sep = input.find('|');
//            if (sep == std::string::npos) {
//                std::string err = "FAIL: Malformed command.";
//                send(clientSock, err.c_str(), static_cast<int>(err.length()), 0);
//                goto cleanup;
//            }
//
//            std::cout << "[REQUEST] " + input << "\n";
//
//            std::string senderLine = trim(input.substr(0, sep));
//            std::string command = trim(input.substr(sep + 1));
//
//            if (senderLine.find("FROM:") != 0) {
//                std::string err = "FAIL: Missing sender IP.";
//                send(clientSock, err.c_str(), static_cast<int>(err.length()), 0);
//                goto cleanup;
//            }
//
//            std::string senderIp = trim(senderLine.substr(5));
//            if (command == "Connected requested") {
//                if (!isTrusted(senderIp)) {
//                    std::cout << "[PROMPT] Connecting from new IP: " << senderIp << ". Allow? (y/n): ";
//                    char choice;
//                    std::cin >> choice;
//                    if (choice == 'y' || choice == 'Y') {
//                        addTrustedIp(senderIp);
//                        std::cout << "[INFO] Added trusted IP: " << senderIp << "\n";
//                        std::string success = "SUCCESS: Connection accepted";
//                        send(clientSock, success.c_str(), static_cast<int>(success.length()), 0);
//                    }
//                    else {
//                        std::string denied = "FAIL: IP not trusted.";
//                        std::cout << "[INFO] Connection refused from: " << senderIp << "\n";
//                        send(clientSock, denied.c_str(), static_cast<int>(denied.length()), 0);
//                        goto cleanup;
//                    }
//                }
//                else {
//                    std::string success = "SUCCESS: Connection accepted";
//                    send(clientSock, success.c_str(), static_cast<int>(success.length()), 0);
//                }
//            }
//            else {
//
//                std::string status;
//                bool delayExec = false;
//                bool success = executeCommand(command, status, delayExec);
//
//                std::string response = (success ? "SUCCESS: " : "FAIL: ") + status;
//                send(clientSock, response.c_str(), static_cast<int>(response.length()), 0);
//
//#ifdef _WIN32
//                if (delayExec && command == "shutdown system") {
//                    system("shutdown /s /f /t 0");
//                }
//                else if (delayExec && command == "restart system") {
//                    system("shutdown /r /f /t 0");
//                }
//#else
//                if (delayExec && command == "shutdown system") {
//                    system("shutdown -h now");
//                }
//                else if (delayExec && command == "restart system") {
//                    system("reboot");
//                }
//#endif
//            }
//        }
//        else {
//            std::cerr << "[WARNING] Failed to receive data or empty command.\n";
//        }
//
//    cleanup:
//#ifdef _WIN32
//        closesocket(clientSock);
//#else
//        close(clientSock);
//#endif
//        std::cout << "[INFO] Client socket closed.\n";
//    }
//
//#ifdef _WIN32
//    closesocket(listenSock);
//    WSACleanup();
//#else
//    close(listenSock);
//#endif
//
//    std::cout << "[INFO] Server shutdown.\n";
//    return 0;
//}