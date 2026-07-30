#pragma once
#include <string>

// outgoing packet
struct AgentData {
    char hostname[32];
    int cpu_usage;
    int ram_usage;
//  int disk_activity;
};
