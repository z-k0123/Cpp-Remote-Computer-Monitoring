#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

#include "packet.h"
#include "SystemMonitor.h"

#define SERVER_IP "127.0.0.1"  // simdilik ayni makine; baska bilgisayardan test edince oranin IP'si olacak
#define PORT 8888

// --- senin zaten yazdigin fonksiyonlar buraya gelecek ---
// double cpuusage() { ... }
// int ramusage() { ... }

// Tek bir NetworkPacket'i server'a gonderir.
// Her cagrida YENI bir soket acilir ve gonderim sonrasi kapatilir,
// cunku server tarafi da her paket icin ayri accept() yapiyor (kalici baglanti degil).
bool sendToServer(const  AgentData& Packet) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Soket olusturulamadi, hata: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Server acik degilse veya port kapaliysa burasi basarisiz olur.
    // Bu NORMAL bir durum (server henuz baslamamis olabilir), bu yuzden
    // programi cokertmiyoruz, sadece false donup bir sonraki denemeyi bekliyoruz.
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Sunucuya baglanilamadi, hata: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return false;
    }

    int bytesSent = send(sock, (char*)&Packet, sizeof(Packet), 0);
    closesocket(sock);

    return bytesSent == sizeof(Packet);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup basarisiz" << std::endl;
        return 1;
    }

    std::cout << "Ajan baslatildi. Sunucuya (" << SERVER_IP << ":" << PORT << ") veri gonderiliyor..." << std::endl;
    std::cout << "Cikmak icin ESC basin." << std::endl;

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
         AgentData Packet{};  // {} ile sifirla, cop veri kalmasin

        // Bilgisayar adini al ve packet.hostname'e guvenli sekilde kopyala.
        char hostname[32];
        DWORD size = sizeof(hostname);
        GetComputerNameA(hostname, &size);
        strncpy_s(Packet.hostname, hostname, sizeof(Packet.hostname) - 1);

        Packet.cpu_usage = (int)cpuusage();   // senin fonksiyonun (double donuyorsa int'e yuvarlanir)
        Packet.ram_usage = ramusage();
     //   Packet.disk_activity = 0;             // henuz eklenmedi

        bool success = sendToServer(Packet);

        std::cout << "CPU: %" << Packet.cpu_usage
                   << "  RAM: %" << Packet.ram_usage
                   << "  Gonderim: " << (success ? "basarili" : "basarisiz") << std::endl;

        // ESC kontrolunu daha sik yapmak icin 2 saniyeyi kucuk parcalara bol
        for (int i = 0; i < 40; ++i) {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
            Sleep(50);
        }
    }

    WSACleanup();
    return 0;
}
