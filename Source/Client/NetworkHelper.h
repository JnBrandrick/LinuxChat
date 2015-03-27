/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: NetworkHelper.h - Contains the definitions of the declarations in NetworkHelper.cpp.
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
#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <string>

#define DEFAULT_PORT 7000

int TCPSocket();
int ServerConnect(int clientSocket, std::string address, int port);
int CleanupSocket(int clientSocket);

void DisplayError(std::string errStr);

#endif
