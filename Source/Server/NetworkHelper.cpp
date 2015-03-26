#include <iostream>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "NetworkHelper.h"

using namespace std;

int TCPSocket()
{
    int listenSocket;

    if((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        DisplayError("Listen socket failed");
        return -1;
    }

    return listenSocket;
}

int SetReuseOpt(int listenSocket)
{
    int argument = 1;

    if(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &argument, sizeof(argument)) == -1)
    {
        DisplayError("Socket option failed");
        return -1;
    }

    return 0;
}

int BindSocket(int listenSocket)
{
    struct sockaddr_in server;

    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenSocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        DisplayError("Bind failed");
        return -1;
    }

    return 0;
}

int ListenClient(int listenSocket)
{
    if(listen(listenSocket, L_QUEUE) == -1)
    {
        DisplayError("Listen failed");
        return -1;
    }

    return 0;
}

int CleanupSocket(int listenSocket)
{
    if(close(listenSocket) < 0)
    {
        DisplayError("Clsoe failed");
        return -1;
    }

    return 0;
}

ClientInfo AcceptClient(int listenSocket)
{
    ClientInfo client;
    sockaddr_in clientAddr;
    unsigned int clientLen = sizeof(clientAddr);

    if((client.Socket = accept(listenSocket, (struct sockaddr *)&clientAddr, &clientLen)) == -1)
    {
        DisplayError("Accept failed");
        return client;
    }

    client.Address = string(inet_ntoa(clientAddr.sin_addr));

    return client;
}

void DisplayOutput(string output)
{
    cout << output << endl;
}

void DisplayError(string errStr)
{
    cerr << errStr << endl;
}
