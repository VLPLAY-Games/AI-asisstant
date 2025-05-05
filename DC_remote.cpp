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
//#include <string.h>
//#define INVALID_SOCKET (-1)
//#define SOCKET_ERROR (-1)
//typedef int SOCKET;
//#endif
//
//#include <iostream>
//#include <string>
//#include <algorithm>
//
//bool executeCommand(const std::string& command, std::string& statusMessage, bool& delayExecution) {
//    std::cout << "[INFO] Received command: \"" << command << "\"" << std::endl;
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
//    else if (command == "Connected requested") {
//        std::cout << "[PROMPT] Connection request received. Do you accept the connection? (y/n): ";
//        char userInput;
//        std::cin >> userInput;
//        if (userInput == 'y' || userInput == 'Y') {
//            statusMessage = "Connection accepted";
//            return true;
//        }
//        else {
//            statusMessage = "Connection declined";
//            return false;
//        }
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
//    else if (command == "Connected requested") {
//        std::cout << "[PROMPT] Connection request received. Do you accept the connection? (y/n): ";
//        char userInput;
//        std::cin >> userInput;
//        if (userInput == 'y' || userInput == 'Y') {
//            statusMessage = "Connection accepted.";
//            return true;
//        }
//        else {
//            statusMessage = "Connection declined.";
//            return false;
//        }
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
//
//int main() {
//    std::cout << "AI Assistant Device Remote Connection v1.1.0\n\n";
//
//#ifdef _WIN32
//    WSADATA wsaData;
//    std::cout << "[INFO] Initializing Winsock...\n";
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "[ERROR] WSAStartup failed.\n";
//        return 1;
//    }
//#endif
//
//    std::cout << "[INFO] Creating socket...\n";
//    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
//    if (listenSock == INVALID_SOCKET) {
//        std::cerr << "[ERROR] Cannot create socket.\n";
//#ifdef _WIN32
//        WSACleanup();
//#endif
//        return 1;
//    }
//
//    sockaddr_in addr{};
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = INADDR_ANY;
//    addr.sin_port = htons(50505);
//
//    std::cout << "[INFO] Binding socket to port 50505...\n";
//    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
//        std::cerr << "[ERROR] Bind failed.\n";
//#ifdef _WIN32
//        closesocket(listenSock);
//        WSACleanup();
//#else
//        close(listenSock);
//#endif
//        return 1;
//    }
//
//    std::cout << "[INFO] Starting to listen for incoming connections...\n";
//    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
//        std::cerr << "[ERROR] Listen failed.\n";
//#ifdef _WIN32
//        closesocket(listenSock);
//        WSACleanup();
//#else
//        close(listenSock);
//#endif
//        return 1;
//    }
//
//    std::cout << "[READY] Server is listening on port 50505.\n";
//
//    while (true) {
//        std::cout << "[WAITING] Waiting for a client to connect...\n";
//        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
//        if (clientSock == INVALID_SOCKET) {
//            std::cerr << "[WARNING] Invalid client socket received. Skipping.\n";
//            continue;
//        }
//
//        std::cout << "[INFO] Client connected.\n";
//
//        char buffer[1024]{};
//        int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//        if (received > 0) {
//            buffer[received] = '\0';
//            std::string command(buffer);
//
//            command.erase(command.begin(), std::find_if(command.begin(), command.end(), [](unsigned char c) {
//                return !std::isspace(c) && c != '.';
//                }));
//            command.erase(std::find_if(command.rbegin(), command.rend(), [](unsigned char c) {
//                return !std::isspace(c) && c != '.';
//                }).base(), command.end());
//
//            std::cout << "[COMMAND] Received: \"" << command << "\"\n";
//
//            std::string status;
//            bool delayExec = false;
//            bool success = executeCommand(command, status, delayExec);
//
//            std::string response = success ? "SUCCESS: " : "FAIL: ";
//            response += status;
//
//            std::cout << "[RESPONSE] Sending back: \"" << response << "\"\n";
//            send(clientSock, response.c_str(), static_cast<int>(response.length()), 0);
//
//            if (success && delayExec) {
//                std::cout << "[ACTION] Executing delayed system command...\n";
//#ifdef _WIN32
//                if (command == "shutdown system") {
//                    system("shutdown /s /f /t 0");
//                }
//                else if (command == "restart system") {
//                    system("shutdown /r /f /t 0");
//                }
//#else
//                if (command == "shutdown system") {
//                    system("shutdown -h now");
//                }
//                else if (command == "restart system") {
//                    system("reboot");
//                }
//#endif
//            }
//        }
//        else {
//            std::cerr << "[WARNING] Failed to receive data or empty command.\n";
//        }
//
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
//
