#include "server.h"
#include "../monitoring_agent/packet.h"
#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8888
#define WAITING_QUEUE 10


std::map<std::string, AgentRecord> agentList;
std::mutex listLock;
bool serverOnline = true;

void handleClient(SOCKET clientSocket) {
    AgentData incomingPacket;

    while (serverOnline) {
        int bytesReceived = recv(clientSocket, (char*)&incomingPacket, sizeof(AgentData), 0);

        if (bytesReceived <= 0) { break;}

        if (bytesReceived == sizeof(AgentData)) {
            std::lock_guard<std::mutex> lock(listLock);
            std::string host(incomingPacket.hostname);

            agentList[host] = AgentRecord{
                host,
                incomingPacket.cpu_usage,
                incomingPacket.ram_usage,
                time(0),
                "OK"
            };
        }
    }

    closesocket(clientSocket);  // bu client kesin kopunca kapat
}


// works in seperate thread. listens TCP port. its starting detached to prevent blocking the main thread
void networkListener() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // start winsock lib

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create IPv4, TCP socket

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);        // change port number to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // listen every network interface

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, WAITING_QUEUE); // WAITING_QUEUE  default: 10

    while (serverOnline) {
        sockaddr_in clientAddr;
        int clientAddr_size = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddr_size);

        if (clientSocket != INVALID_SOCKET) {
            // her client icin ayri bir thread baslat, boylece accept() hemen
            // bir sonraki client'i beklemeye devam edebilir
            std::thread(handleClient, clientSocket).detach();
        }
    }
    closesocket(serverSocket);
    WSACleanup();
}

void createTable() {
    std::lock_guard<std::mutex> lock(listLock); // prevent listener working while creating table

    std::cout << "====================================================================================================" << std::endl;
    std::cout << std::left
              << std::setw(18) << " HOSTNAME"
              << std::setw(12) << "CPU"
              << std::setw(12) << "RAM"
              << std::setw(12) << "DISK"
              << std::setw(16) << "STATUS"
              << "LAST UPDATE" << std::endl;
    std::cout << "====================================================================================================" << std::endl;

    int totalAgent = agentList.size();
    int warnings = 0;
    time_t currenttime = time(0);

    for (auto& [host, agent] : agentList) {
        double timePassed = difftime(currenttime, agent.last_update);

        // set agent OFFLINE if there's no data for 15 secs
        if (timePassed > 15 && agent.status != "OFFLINE") {
            agent.status = "OFFLINE";
            agent.cpu_usage = 0;
            agent.ram_usage = 0;
        }

        std::string timeText = (timePassed < 2) ? "now" : std::to_string((int)timePassed) + " sn once";

        std::cout << " " << std::left << std::setw(17) << agent.hostname;
        std::cout << "%" << std::left << std::setw(11) << agent.cpu_usage;
        std::cout << "%" << std::left << std::setw(11) << agent.ram_usage;
        std::cout << std::left << std::setw(16) << agent.status;
        std::cout << timeText << std::endl;

        if (agent.status != "OK") warnings++;
    }

    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Toplam Agent: " << totalAgent << std::endl;
    std::cout << " Uyari: " << warnings << std::endl;
    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Sunucu 8888 portundan verileri dinliyor. Cikmak icin ESC..." << std::endl;
}
