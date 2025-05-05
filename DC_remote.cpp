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
//    delayExecution = false;
//
//#ifdef _WIN32
//    if (command == "run explorer") {
//        int ret = system("explorer");
//        statusMessage = "Explorer launched.";
//        return ret == 0;
//    }
//    else if (command == "open browser") {
//        int ret = system("start chrome");
//        statusMessage = "Browser opened.";
//        return ret == 0;
//    }
//    else if (command == "shutdown system") {
//        statusMessage = "System shutdown will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "restart system") {
//        statusMessage = "System restart will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "status") {
//        statusMessage = "System is running normally on Windows.";
//        return true;
//    }
//#else
//    if (command == "run explorer") {
//        int ret = system("xdg-open .");
//        statusMessage = "File manager launched.";
//        return ret == 0;
//    }
//    else if (command == "open browser") {
//        int ret = system("xdg-open http://");
//        statusMessage = "Browser opened.";
//        return ret == 0;
//    }
//    else if (command == "shutdown system") {
//        statusMessage = "System shutdown will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "restart system") {
//        statusMessage = "System restart will be executed.";
//        delayExecution = true;
//        return true;
//    }
//    else if (command == "status") {
//        statusMessage = "System is running normally on Linux.";
//        return true;
//    }
//#endif
//
//    statusMessage = "Unknown command.";
//    return false;
//}
//
//int main() {
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
//        std::cerr << "Cannot create socket\n";
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
//    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
//        std::cerr << "Bind failed\n";
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
//        std::cerr << "Listen failed\n";
//#ifdef _WIN32
//        closesocket(listenSock);
//        WSACleanup();
//#else
//        close(listenSock);
//#endif
//        return 1;
//    }
//
//    std::cout << "Server is listening on port 50505...\n";
//
//    while (true) {
//        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
//        if (clientSock == INVALID_SOCKET) continue;
//
//        char buffer[1024]{};
//        int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//        if (received > 0) {
//            buffer[received] = '\0';
//            std::string command(buffer);
//            command.erase(command.size() - 2);
//            std::cout << "Received command: " << command << std::endl;
//
//            std::string status;
//            bool delayExec = false;
//            bool success = executeCommand(command, status, delayExec);
//
//            std::string response = success ? "SUCCESS: " : "FAIL: ";
//            response += status;
//
//            send(clientSock, response.c_str(), static_cast<int>(response.length()), 0);
//
//            // Отложенное выполнение после отправки ответа
//            if (success && delayExec) {
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
//
//#ifdef _WIN32
//        closesocket(clientSock);
//#else
//        close(clientSock);
//#endif
//    }
//
//#ifdef _WIN32
//    closesocket(listenSock);
//    WSACleanup();
//#else
//    close(listenSock);
//#endif
//
//    return 0;
//}
