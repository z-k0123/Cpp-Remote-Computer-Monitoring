#pragma once
#include <windows.h>
#include <cstdint>

int ramusage();
int  cpuusage();
float DiskActivity();

int diskusage(const char* driver = "C:\\");
void wipe();
void ShowCursor(bool showFlag);
void SetColor(int textColor, int bgColor);
