#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <strings.h>

#define DEFAULT_PORT 7000
#define BUFF_LEN 255

struct SocketInfo
{
    int Listen;
    char *Address;
    int Port;
    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;
    int ClientList[FD_SETSIZE];
};

SocketInfo *InitServer();
int ServerLoop(SocketInfo *lpSockInfo);
int CleanupSocket(SocketInfo *lpSockInfo);
void SignalHandler(int sigNum);

#endif
