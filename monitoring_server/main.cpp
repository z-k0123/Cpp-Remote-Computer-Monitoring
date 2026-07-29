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

