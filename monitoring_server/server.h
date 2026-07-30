#pragma once
#define HOST_NAME_LENGTH 32

// incoming packet
struct AgentData {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
    time_t last_update;
    std::string status;}

void networkListener();
void createTable();

