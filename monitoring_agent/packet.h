#pragma once
#include <string>

// outgoing packet
struct AgentData {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
};
