#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "server.h"

int main() {
    system("cls"); // clear screen

    // Dinleyici thread'i arka planda baslat. detach(): main thread bu thread'i
    // bekleyip "join" etmeyecek, kendi basina, bagimsiz calisacak.
    std::thread listener(networkListener);
    listener.detach();

    // ESC'ye basilana kadar tabloyu periyodik olarak yeniden ciz.
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(hOut, coord); // yazmadan once imleci ekranin basina al

        createTable();

        // Sleep(1000) yerine 50ms'lik 20 parcaya bolunmus bekleme:
        // boylece ESC'ye basildiginda tepki suresi ~1 saniye yerine ~50ms oluyor.
        for (int i = 0; i < 20; ++i) {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
            Sleep(50);
        }
    }

    serverOnline = false; // networkListener dongusune "artik kapat" sinyali
    system("cls");
    std::cout << "Sunucu kapatildi." << std::endl;
    return 0;
}
