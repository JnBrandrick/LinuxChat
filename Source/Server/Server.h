#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#define DEFAULT_PORT 7000
#define BUFF_LEN 256

struct SocketInfo
{
    int Listen;
    int Port;
    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;
    int ClientList[FD_SETSIZE];
};

SocketInfo *InitServer();
int ServerLoop(SocketInfo *lpSockInfo);
int CleanupSocket(SocketInfo *lpSockInfo);

#endif
