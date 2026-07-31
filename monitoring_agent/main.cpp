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

    SOCKET clientSocket = INVALID_SOCKET;

    std::cout << "Sunucuya baglanmaya calisiliyor..." << std::endl;

    // Sunucu IP ve Port bilgisi
    while (!connectToServer(clientSocket, "127.0.0.1", 8888)) {
        std::cerr << "Sunucuya baglanmaya calisiliyor..." << std::endl;
        Sleep(1000);
    }

    std::cout << "Sunucuya basariyla baglanildi!" << std::endl;

    while (true) {
        AgentData packet{};  // {} ile sifirla, cop veri kalmasin

        char hostname[HOSTNAME_LEN];
        DWORD size = sizeof(hostname);
        GetComputerNameA(hostname, &size);
        strncpy_s(packet.hostname, hostname, sizeof(packet.hostname) - 1);

        packet.ram_usage = ramusage();
        packet.cpu_usage = cpuusage();
      //  packet.disk_activity = 0;

        if (!sendData(clientSocket, packet)) {
            std::cerr << "Baglanti koptu, yeniden baglaniliyor..." << std::endl;
            cleanupSocket(clientSocket);

            // baglanti kopunca tekrar baglanmayi dene
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

