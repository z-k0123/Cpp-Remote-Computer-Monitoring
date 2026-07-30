#pragma once
#include <winsock2.h>
#include "packet.h"
#pragma comment(lib, "ws2_32.lib")


bool connectToServer(SOCKET& clientSocket, const char* ip, int port);

bool sendData(SOCKET clientSocket, const AgentData& packet);

void cleanupSocket(SOCKET clientSocket);
