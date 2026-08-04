#pragma once
#include <string>
#include <ctime>

// incoming packet
// status info for every agent
// adding status and and last_update to the network packet
struct AgentRecord {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
    float disk_activity;
    int disk_usage;
    time_t time_passed;
    std::string status;
};

void networkListener();
void createTable();
extern bool serverOnline;
char changeColor();

