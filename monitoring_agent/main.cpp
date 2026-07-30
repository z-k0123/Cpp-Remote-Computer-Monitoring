#include "SystemMonitor.h"
#include "NetworkClient.h"
#include <iostream>
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
    if (!connectToServer(clientSocket, "127.0.0.1", 8888)) {
        std::cerr << "Baglanti basarisiz. Program kapatiliyor." << std::endl;
        Sleep(2000);
        return 1;
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

        sendData(clientSocket, packet);


                // 1 saniye bekleme
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            cleanupSocket(clientSocket);
            return 0;
}
