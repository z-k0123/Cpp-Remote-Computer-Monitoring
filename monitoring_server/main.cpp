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
    std::cout << "Sunucu kapatildi." << "\n";
    Sleep(1000);
    return 0;
}
