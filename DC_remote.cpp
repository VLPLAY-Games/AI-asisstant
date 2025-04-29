//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <winsock2.h>
//#include <iostream>
//
//#pragma comment(lib, "ws2_32.lib")
//
//int main() {
//    WSADATA wsaData;
//    WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (listenSock == INVALID_SOCKET) {
//        std::cerr << "Cannot create Socket\n";
//        return 1;
//    }
//
//    sockaddr_in addr{};
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = INADDR_ANY;
//    addr.sin_port = htons(50505); // Порт сервера
//
//    bind(listenSock, (sockaddr*)&addr, sizeof(addr));
//    listen(listenSock, SOMAXCONN);
//
//    std::cout << "Server is listening port 50505...\n";
//
//    while (true) {
//        SOCKET clientSock = accept(listenSock, nullptr, nullptr);
//        if (clientSock == INVALID_SOCKET) continue;
//
//        char buffer[1024]{};
//        int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
//        if (received > 0) {
//            buffer[received] = '\0';
//            std::cout << "Received command: " << buffer << std::endl;
//
//            std::string response = "OK: ";
//            response += buffer;
//            send(clientSock, response.c_str(), static_cast<int>(response.length()), 0);
//        }
//
//        closesocket(clientSock);
//    }
//
//    closesocket(listenSock);
//    WSACleanup();
//    return 0;
//}
