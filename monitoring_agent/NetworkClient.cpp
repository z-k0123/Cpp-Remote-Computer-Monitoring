#pragma once
#include <string>
#include "NetworkClient.h"
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


bool connectToServer(SOCKET& clientSocket, const char* ip, int port){
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0){
        std::cerr << "WSAStartup failed. Error code: " << WSAGetLastError() << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSocket == INVALID_SOCKET){
        std::cerr << "Couldn't create socket. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Couldn't connect to the server. Error code: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    return true;
}

bool sendData(SOCKET clientSocket, const AgentData& packet){
    int bytesSent = send(clientSocket, reinterpret_cast<const char*>(&packet), sizeof(AgentData), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Couldn't send data. Error code: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

void cleanupSocket(SOCKET clientSocket){
    if(clientSocket != INVALID_SOCKET){
        closesocket(clientsocket);
    }
    WSACleanup();
}
