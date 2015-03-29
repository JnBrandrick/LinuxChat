/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: NetworkHelper.cpp - All of the helper functions for setting up a TCP server socket.
--
-- FUNCTIONS:
--  int TCPSocket()
--  int ServerConnect   (int clientSocket)
--  int CleanupSocket   (int clientSocket)
--  void DisplayError   (string errStr)
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

#include <netdb.h>
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
--  This function creates a TCP socket with no flags set.
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ServerConnect
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int ServerConnect (int clientSocket, string address, int port)
--
-- PARAMETERS:
--  clientSocket -> A TCP client socket
--  address      -> Dotted decimal address for the server
--  port         -> Port of the server
--
-- RETURNS: int
--  0  -> Successfully connected to a TCP server
-- -1  -> Server connection faield
--
-- NOTES:
--  This fucntion creates a TCP connection between a client and server.
----------------------------------------------------------------------------------------------------------------------*/
int ServerConnect(int clientSocket, string address, int port)
{
    struct sockaddr_in server;
    struct hostent *host;

    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((host = gethostbyname(address.c_str())) == NULL)
    {
        DisplayError("GetHostByName failed");
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: CleanupSocket
--
-- DATE: March 21, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int CleanupSocket (int clientSocket)
--
-- PARAMETERS:
--  listenSocket -> A TCP client socket
--
-- RETURNS: int
--  0  -> Socket closed successfully
-- -1  -> Clsoe failed
--
-- NOTES:
--  This function closes a TCP socket.
----------------------------------------------------------------------------------------------------------------------*/
int CleanupSocket(int clientSocket)
{
    if(close(clientSocket) < 0)
    {
        DisplayError("Close failed");
        return -1;
    }

    return 0;
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
    cerr << "Error: " << errStr << endl;
}
