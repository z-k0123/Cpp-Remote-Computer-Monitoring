#pragma once
#include <winsock2.h>
#include "packet.h"


bool connectToServer(SOCKET& clientSocket, const char* ip, int port);

bool sendData(SOCKET clientSocket, const AgentData& packet);

void cleanupSocket(SOCKET clientSocket);
