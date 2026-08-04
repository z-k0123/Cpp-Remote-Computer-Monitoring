#include <windows.h>
#include <fileapi.h>
#include <pdh.h>
#include <cstdint>
#include <cmath>
#include <winioctl.h>
#include <ioapiset.h>
#include <iostream>
#include <cmath>
#include "SystemMonitor.h"
#include <iomanip>


// find ram usage percentage

int ramusage () {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);  // update memInfo with current ram information
    return memInfo.dwMemoryLoad;    // return current ram usage
}

// helping function: convert filetime (two 32 bits) to one 64 bit integer

static uint64_t FILETIME_to_64(const FILETIME &ft){
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime; // equal filetime's "low part" to the low part of the ularge_integer
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;            // return the whole (low part + high part as one --> quadpart)
}

// find cpu usage percentage
int cpuusage () {
    FILETIME idletime1, kerneltime1, usertime1;
    FILETIME idletime2, kerneltime2, usertime2;

    GetSystemTimes(&idletime1, &kerneltime1, &usertime1);

    Sleep(1000);

    GetSystemTimes(&idletime2, &kerneltime2, &usertime2);

    uint64_t idle1 = FILETIME_to_64(idletime1);
    uint64_t kernel1 = FILETIME_to_64(kerneltime1);
    uint64_t user1 = FILETIME_to_64(usertime1);

    uint64_t idle2 = FILETIME_to_64(idletime2);
    uint64_t kernel2 = FILETIME_to_64(kerneltime2);
    uint64_t user2 = FILETIME_to_64(usertime2);

    uint64_t totalIdle = idle2 - idle1;
    uint64_t totalKernel = kernel2 - kernel1;
    uint64_t totalUser = user2 - user1;

    uint64_t totalSystemTime = totalKernel + totalUser; // idle time is included inside kernel time
    if (totalSystemTime == 0) return 0.0;

    int Percentage = round((double)((totalSystemTime - totalIdle)) / totalSystemTime * 100);

    return Percentage;

}

// move cursor to the 0,0  -- "wipes" screen
void wipe() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD pos = {0, 0};

    SetConsoleCursorPosition(out, pos);
}

// set the cursor visibility
void ShowCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

// find disk usage percentage


int diskusage(const char* driver){
    ULARGE_INTEGER FreeBytesAvailableToCaller;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;

    GetDiskFreeSpaceExA(driver, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);

    uint64_t totalBytes = TotalNumberOfBytes.QuadPart;
    uint64_t freeBytes = FreeBytesAvailableToCaller.QuadPart;

    int Disk_Percantage = ((double)(totalBytes - freeBytes) / totalBytes) * 100;

    return Disk_Percantage;

}

float DiskActivity(){
    DISK_PERFORMANCE diskperf;
    HANDLE diskHandle = CreateFile(
    "\\\\.\\PhysicalDrive0",
    0,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
    );

    DWORD bytesReturned;

    BOOL success = DeviceIoControl(diskHandle,
    IOCTL_DISK_PERFORMANCE,
    nullptr,
    0,
    &diskperf,
    sizeof(diskperf),
    &bytesReturned, // getting BytesWritten value here
    nullptr
    );

    if(!success){
        std::cout << "device io control fail. error: " << GetLastError() << std::endl;
        CloseHandle(diskHandle);
    }

    uint64_t disk_write1 = diskperf.BytesWritten.QuadPart;
    Sleep(1000);

    BOOL success2 = DeviceIoControl(diskHandle,
    IOCTL_DISK_PERFORMANCE,
    nullptr,
    0,
    &diskperf,
    sizeof(diskperf),
    &bytesReturned, // getting BytesWritten value here
    nullptr
    );

    if(!success2){
        std::cout << "device io control fail. error: " << GetLastError() << std::endl;
        CloseHandle(diskHandle);
    }

    uint64_t disk_write2 = diskperf.BytesWritten.QuadPart;

    float disk_write = (disk_write2 - disk_write1) / std::pow(2, 20);

    return disk_write;

 // debug  std::cout << "returned bytes: " << std::setprecision(2) << disk_write << " MB/s" << std::endl;

}

