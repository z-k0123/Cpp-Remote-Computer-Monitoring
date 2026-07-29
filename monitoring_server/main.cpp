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
}
