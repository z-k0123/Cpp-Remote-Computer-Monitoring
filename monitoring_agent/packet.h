#pragma once
#include <string>
#define HOSTNAME_LEN 32

#pragma pack(push, 1)
struct AgentData {
    char hostname[HOSTNAME_LEN];
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
};

#pragma pack(pop)
