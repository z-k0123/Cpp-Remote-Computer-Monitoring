#pragma once
#include <string>
#include <ctime>

// incoming packet
// status info for every agent
// adding status and and last_update to the network packet
struct AgentData {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
    time_t last_update;
    std::string status;
};

void networkListener();
void createTable();
extern bool serverOnline;

