#pragma once
#include <string>


#pragma pack(push, 1)
struct AgentData {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
};

#pragma pack(pop)
