#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>

#define DEFAULT_PORT 7000
#define BUFF_LEN 256

const std::string TITLE_BANNER = "======================\n"
                                 "| Linux Chat Program |\n"
                                 "======================\n";

struct SocketInfo
{
    int Socket;
    char *Address;
    int Port;
    struct hostent *Host;
    struct sockaddr_in ServerAddr;
    std::string Output;
};

SocketInfo *InitClient(char *address, int port);
int MessageLoop(SocketInfo *lpSockInfo);
void* ReceiveLoop(void* param);
int CleanupSocket(SocketInfo *lpSockInfo);

#endif
