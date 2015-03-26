#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <string>

#define DEFAULT_PORT 7000

int TCPSocket();
int ServerConnect(int clientSocket, std::string address, int port);
int CleanupSocket(int clientSocket);

void DisplayOutput(std::string output);
void DisplayError(std::string errStr);

#endif
