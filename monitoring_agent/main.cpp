#include "SystemMonitor.h"
#include "NetworkClient.h"
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <cstdint>
#include <iomanip>
#include <thread>
#include <ctime>
#define HOSTNAME_LEN 32


int main() {
    DiskActivity();
    SOCKET clientSocket = INVALID_SOCKET;

    while (!connectToServer(clientSocket, "127.0.0.1", 8888)) {
        std::cerr <<  "Trying to connect to the server..." << std::endl;
        Sleep(1000);
    }

    std::cout << "Connected to the server!" << std::endl;

    while (true) {
        AgentData packet{};  // reset packet

        char hostname[HOSTNAME_LEN];
        DWORD size = sizeof(hostname);
        GetComputerNameA(hostname, &size);
        strncpy_s(packet.hostname, hostname, sizeof(packet.hostname) - 1);

        packet.ram_usage = ramusage();
        packet.cpu_usage = cpuusage();
        packet.disk_activity = DiskActivity();
        packet.disk_usage = diskusage();

        if (!sendData(clientSocket, packet)) {
            std::cerr << "Connection failed, trying again..." << std::endl;
            cleanupSocket(clientSocket);

            // try again if connection fails
            while (!connectToServer(clientSocket, "127.0.0.1", 8888)) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            continue;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    cleanupSocket(clientSocket);
    return 0;
}

