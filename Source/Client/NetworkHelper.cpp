#include <iostream>
#include <cstdlib>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "NetworkHelper.h"

using namespace std;

int TCPSocket()
{
    int clientSocket;

    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        DisplayError("Client socket failed");
        return -1;
    }

    return clientSocket;
}

int ServerConnect(int clientSocket, string address, int port)
{
    struct sockaddr_in server;
    struct hostent *host;

    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((host = gethostbyname(address.c_str())) == NULL)
    {
        DisplayError("GetHostByName sailed");
        return -1;
    }
    memcpy((char *)&server.sin_addr, host->h_addr, host->h_length);

    if (connect (clientSocket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		DisplayError("Connect failed");
		return -1;
	}

    return 0;
}

int CleanupSocket(int listenSocket)
{
    if(close(listenSocket) < 0)
    {
        DisplayError("Close failed");
        return -1;
    }

    return 0;
}

void DisplayError(string errStr)
{
    cerr << errStr << endl;
}
