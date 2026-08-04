#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "server.h"

int main() {
    std::cout << "========================= WARNING ========================\n";
    std::cout << "Resizing this window may break text formatting and layouts.\n";
    std::cout << "==========================================================\n";
    std::cout << "High values may be normal depending on workload.\n";
    std::cout << "Investigate sustained high usage instead of isolated spikes.\n";
    Sleep(3000);
    system("cls"); // clear screen

    // start listener thread
    std::thread listener(networkListener);
    listener.detach();

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(hOut, coord);

        createTable();

        // reducing waiting time to 50 ms when pressed ESC
        for (int i = 0; i < 20; ++i) {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
            Sleep(50);
        }
    }

    serverOnline = false;
    system("cls");
    std::cout << "Server closed." << "\n";
    Sleep(1000);
    return 0;
}
