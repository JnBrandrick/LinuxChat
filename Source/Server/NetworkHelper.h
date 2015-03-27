/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Server - The server half of a Linux-based chat application.
--
-- SOURCE FILE: NetworkHelper.h - Contains the definitions of the declarations in NetworkHelper.cpp.
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
