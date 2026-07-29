#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "NetworkPacket.h"

#define PORT 8888

struct AgentData{
    std::string hostname;
    int cpu_usage;
    int ram_usage;
    int disk_activity;
    time_t last_update;
    std::string status;
};

std::map <std::string, AgentData> agentList;
std::mutex listLock;
bool serverOnline = true;

void networkListener(){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0); // use ipv4, tcp

    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT); //converting port number to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY; // uses any ip address

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 10);

    while(serverOnline){
        sockaddr_in clientAddr;

        int clientAddr_size = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket,(struct sockaddr*)&clientAddr, &clientAddr_size);

        if (clientSocket != INVALID_SOCKET) {
            NetworkPacket incomingPacket;

            int bytesReceived = recv(clientSocket, (char*)&incomingPacket, sizeof(NetworkPacket), 0);

            if (bytesReceived == sizeof(NetworkPacket)){ // check if all of package are received
                std::lock_guard<std::mutex> lock(listLock);

                std::string host(incomingPacket.hostname);
                agentList[host] = {
                    host,
                    incomingPacket.cpu_usage,
                    incomingPacket.ram_usage,
                   // incomingPacket.diskActivity,
                    time(0),
                };
            }
            closesocket(clientSocket);
        }
    }
    closesocket(serverSocket);
    WSACleanup();
}

void tabloyuCiz() {
    std::lock_guard<std::mutex> kilit(listLock);

    std::cout << "====================================================================================================" << std::endl;
    std::cout << std::left
              << std::setw(18) << " HOSTNAME"
              << std::setw(12) << "CPU"
              << std::setw(12) << "RAM"
              << std::setw(12) << "DISK"
              << std::setw(16) << "STATUS"
              << "LAST UPDATE" << std::endl;
    std::cout << "====================================================================================================" << std::endl;

    int toplamAgent = agentList.size();
    int uyariSayisi = 0;
    time_t suAn = time(0);

    for (auto& [host, agent] : agentList) {
        double gecenSure = difftime(suAn, agent.last_update);

        if (gecenSure > 15 && agent.status != "OFFLINE") {
            agent.status = "OFFLINE";
        }

        std::string zamanMetni = (gecenSure < 2) ? "simdi" : std::to_string((int)gecenSure) + " sn once";

        std::cout << " " << std::left << std::setw(17) << agent.hostname;
        std::cout << "%" << std::left << std::setw(11) << agent.cpu_usage;
        std::cout << "%" << std::left << std::setw(11) << agent.ram_usage;
       // std::cout << "%" << std::left << std::setw(11) << agent.diskActivity;
        std::cout << std::left << std::setw(16) << agent.status;
        std::cout << zamanMetni << std::endl;

        if (agent.status != "OK") uyariSayisi++;
    }

    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Toplam Agent: " << toplamAgent << std::endl;
    std::cout << " Uyari: " << uyariSayisi << std::endl;
    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Sunucu 8888 portundan verileri dinliyor. Cikmak icin ESC..." << std::endl;
}

int main() {
    system("cls");

    std::thread listener(networkListener);
    listener.detach();

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(hOut, coord);

        tabloyuCiz();

        for (int i = 0; i < 20; ++i) {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
            Sleep(50);
        }
    }

    serverOnline = false;
    system("cls");
    std::cout << "Sunucu kapatildi." << std::endl;
    return 0;
}
