#pragma once
#define HOST_NAME_LENGTH 32

struct NetworkPacket {
    char hostname[HOST_NAME_LENGTH];
    int cpu_usage;
    int ram_usage;
    int disk_activity;
};

