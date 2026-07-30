#include "server.h"
#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8888

// status info for every agent
// adding status and and last_update to the network packet
struct AgentData {
    std::string hostname;
    int cpu_usage;
    int ram_usage;
    int disk_activity;
    time_t last_update;
    std::string status;}


std::map<std::string, AgentData> agentList;
std::mutex listLock;

// Sunucu dongusunu kontrollu kapatmak icin bayrak (main'de ESC ile false yapiliyor).
bool serverOnline = true;

// Ayri bir thread'de calisir: TCP portu dinler, gelen her paketi agentList'e islerler.
// Bu fonksiyon main thread'i BLOKLAMAMASI icin main()'de detach edilerek baslatiliyor.
void networkListener() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // Winsock kutuphanesini baslat (2.2 surumu)

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP soket olustur

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);        // port numarasini network byte order'a cevir
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // makinedeki tum network arayuzlerinden dinle

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 10); // bekleyen baglanti kuyrugu boyutu: 10

    while (serverOnline) {
        sockaddr_in clientAddr;
        int clientAddr_size = sizeof(clientAddr);

        // accept() bir istemci baglanana kadar burada BLOKLAR (bekler).
        // Bu yuzden serverOnline=false oldugunda thread hemen kapanmaz,
        // bir sonraki baglanti gelene kadar (ya da timeout'a kadar) burada takili kalabilir.
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddr_size);

        if (clientSocket != INVALID_SOCKET) {
            NetworkPacket incomingPacket;

            // Ajanin gonderdigi binary paketi oldugu gibi struct'a oku.
            int bytesReceived = recv(clientSocket, (char*)&incomingPacket, sizeof(NetworkPacket), 0);

            // Paketin TAMAMI gelmis mi kontrol et (parcali/eksik veri isleme alma).
            if (bytesReceived == sizeof(NetworkPacket)) {
                std::lock_guard<std::mutex> lock(listLock); // createTable ile ayni anda yazmayi engelle

                std::string host(incomingPacket.hostname);

                // map[key] = {...}: hostname zaten varsa GUNCELLER, yoksa YENI ekler.
                agentList[host] = {
                    host,
                    incomingPacket.cpu_usage,
                    incomingPacket.ram_usage,
                    0,          // disk_activity - henuz ajan tarafinda yok
                    time(0),    // bu paketin geldigi an (offline hesaplamasi icin referans)
                    "OK"        // yeni veri geldi -> agent kesinlikle online, status'u resetle
                };
            }
            closesocket(clientSocket); // bu baglantiyi kapat, bir sonraki paket icin yeni baglanti gelecek
        }
    }
    closesocket(serverSocket);
    WSACleanup();
}

// Ekrandaki tabloyu, agentList'in o anki durumuna gore ciz.
// main() dongusunde surekli cagriliyor; SetConsoleCursorPosition ile ekran silinmeden
// ustune yazildigi icin flicker (goz kirpma) olmuyor.
void createTable() {
    std::lock_guard<std::mutex> lock(listLock); // okurken listener'in yazmasini engelle

    std::cout << "====================================================================================================" << std::endl;
    std::cout << std::left
              << std::setw(18) << " HOSTNAME"
              << std::setw(12) << "CPU"
              << std::setw(12) << "RAM"
              << std::setw(12) << "DISK"
              << std::setw(16) << "STATUS"
              << "LAST UPDATE" << std::endl;
    std::cout << "====================================================================================================" << std::endl;

    int totalAgent = agentList.size();
    int warnings = 0;
    time_t currenttime = time(0);

    // Butun ajanlari sirayla yazdir (map zaten hostname'e gore alfabetik sirali gelir)
    for (auto& [host, agent] : agentList) {
        double timePassed = difftime(currenttime, agent.last_update);

        // 15 saniyedir veri gelmemisse ajani OFFLINE isaretle.
        // NOT: agent tekrar veri gonderirse networkListener zaten status="OK" yapiyor,
        // bu yuzden burada tersini (OK'a donme) yapmaya gerek yok.
        if (timePassed > 15 && agent.status != "OFFLINE") {
            agent.status = "OFFLINE";
        }

        std::string timeText = (timePassed < 2) ? "now" : std::to_string((int)timePassed) + " sn once";

        std::cout << " " << std::left << std::setw(17) << agent.hostname;
        std::cout << "%" << std::left << std::setw(11) << agent.cpu_usage;
        std::cout << "%" << std::left << std::setw(11) << agent.ram_usage;
        std::cout << std::left << std::setw(16) << agent.status;
        std::cout << timeText << std::endl;

        if (agent.status != "OK") warnings++;
    }

    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Toplam Agent: " << totalAgent << std::endl;
    std::cout << " Uyari: " << warnings << std::endl;
    std::cout << "====================================================================================================" << std::endl;
    std::cout << " Sunucu 8888 portundan verileri dinliyor. Cikmak icin ESC..." << std::endl;
}

