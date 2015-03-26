#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <string>

#define DEFAULT_PORT 7000
#define L_QUEUE 5

struct ClientInfo
{
    int Socket;
    std::string Address;
};

int TCPSocket();
int SetReuseOpt(int listenSocket);
int BindSocket(int listenSocket);
int ListenClient(int listenSocket);
int CleanupSocket(int listenSocket);
ClientInfo AcceptClient(int listenSocket);

void DisplayOutput(std::string output);
void DisplayError(std::string errStr);

#endif
