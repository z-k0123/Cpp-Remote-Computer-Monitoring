#include "SystemMonitor.h"
#include <iostream>
#include <windows.h>
#include <cstdint>
#include <iomanip>

int main() {
    system("cls");
    system("color 1F");
    ShowCursor(false);
    while (GetAsyncKeyState(VK_ESCAPE)==0) {
        wipe();

        unsigned int currentRAM = ramusage();
        unsigned int currentDisk = diskusage();
        int  currentCPU = cpuusage();


        std::cout << "=========================================" << '\n';
        std::cout << "        WINDOWS CANLI MONITORU           " << '\n';
        std::cout << "=========================================" << '\n';

        std::cout << " CPU Kullanimi : %" << currentCPU << "         " << '\n';
        std::cout << " RAM Kullanimi : %" << currentRAM << "         " << '\n';
        std::cout << " Disk Kullanimi : %" << currentDisk << "         " << '\n';
        std::cout << "=========================================" << '\n';
        std::cout << " Cikmak icin esc basin.               " << '\n';

        // 1000 ms uykuyu 20 x 50 ms parçalara bölüyoruz
        for (int i = 0; i < 20; ++i) {
            // ESC'ye basýldýðý an 1 saniyenin dolmasýný bekleme!
            if (GetAsyncKeyState(VK_ESCAPE)) {
                return -1.0; // ESC'ye basýldýðýný temsil eden özel bir deðer döndür
            }
        Sleep(50);

        if (currentCPU < 0) {
            break;
        }
}

    }
    ShowCursor(true);

    return 0;
}
