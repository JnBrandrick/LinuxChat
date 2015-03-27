/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Server - The server half of a Linux-based chat application.
--
-- SOURCE FILE: NetworkHelper.cpp - All of the helper functions for setting up a TCP server socket.
--
-- FUNCTIONS:
--  int TCPSocket()
--  int SetReuseOpt         (int listenSocket)
--  int BindSocket          (int listenSocket)
--  int ListenClient        (int listenSocket)
--  int CleanupSocket       (int listenSocket)
--  ClientInfo AcceptClient (int listenSocket)
--  void DisplayOutput      (string output)
--  void DisplayError       (string errStr)
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- NOTES:
--  Every function in this file is a wrapper function for TCP server setup functions.
----------------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "NetworkHelper.h"

using namespace std;

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: TCPSocket
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int TCPSocket (void)
--
-- RETURNS: int
--  0  -> Successfully created a TCP socket
-- -1  -> TCP socket creation failed
--
-- NOTES:
--  This fucntion creates a TCP socket with no flags set.
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SetReuseOpt
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int SetReuseOpt (int listenSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP server socket
--
-- RETURNS: int
--  0  -> Sockets port set to reuseable
-- -1  -> Setsockopt failed
--
-- NOTES:
--  This function sets the sockets port to reuseable.
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: BindSocket
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int BindSocket (int listenSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP server socket
--
-- RETURNS: int
--  0  -> Socket successfully bound to port
-- -1  -> Binding failed
--
-- NOTES:
--  This function binds a TCP server socket to a port.
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ListenClient
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int ListenClient (int listenSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP server socket
--
-- RETURNS: int
--  0  -> Listen running successfully
-- -1  -> Listen failed
--
-- NOTES:
--  This function makes a TCP server socket listen for incoming connections.
----------------------------------------------------------------------------------------------------------------------*/
int ListenClient(int listenSocket)
{
    if(listen(listenSocket, L_QUEUE) == -1)
    {
        DisplayError("Listen failed");
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: CleanupSocket
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int CleanupSocket (int listenSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP server socket
--
-- RETURNS: int
--  0  -> Socket closed successfully
-- -1  -> Close failed
--
-- NOTES:
--  This function closes a TCP socket.
----------------------------------------------------------------------------------------------------------------------*/
int CleanupSocket(int listenSocket)
{
    if(close(listenSocket) < 0)
    {
        DisplayError("Clsoe failed");
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AcceptClient
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: ClientInfo AcceptClient (int listenSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP server socket
--
-- RETURNS: int
--  client  -> Structure containing the socket and address of the connected client
--  NULL    -> Returned if accept failed
--
-- NOTES:
--  This function displays text on the terminal.
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayOutput
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: void DisplayOutput (string output)
--
-- PARAMETERS:
--  output -> Text to be displayed on the terminal
--
-- RETURNS: void
--
-- NOTES:
--  This function displays text on the terminal on stdout.
----------------------------------------------------------------------------------------------------------------------*/
void DisplayOutput(string output)
{
    cout << output << endl;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayError
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: void DisplayError (string errStr)
--
-- PARAMETERS:
--  output -> Error to be displayed on the terminal
--
-- RETURNS: void
--
-- NOTES:
--  This function displays errors on the terminal on stderr.
----------------------------------------------------------------------------------------------------------------------*/
void DisplayError(string errStr)
{
    cerr << errStr << endl;
}
