/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Server - The server half of a Linux-based chat application.
--
-- SOURCE FILE: Server.h - Contains the definitions of the declarations in Server.cpp.
--
-- FUNCTIONS:
--  int InitServer          ()
--  int ServerLoop          (int listenSocket)
--  int RecvClientMessage   (vector<ClientInfo> &clientList, fd_set *allDes, fd_set *tempDes, int numReady)
--  string Count            (string address)
--  void SignalHandler      (int sigNum)
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- NOTES:
--  This file contains all of the constants and function prototypes for Server.cpp.
----------------------------------------------------------------------------------------------------------------------*/
#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <unistd.h>

#include "NetworkHelper.h"

#define BUFF_LEN 256
#define EOT 4;

int InitServer();
int ServerLoop(int listenSocket);
void RecvClientMessage(fd_set *allDes, fd_set *tempDes, int numReady);
std::string Count(std::string address);
void SignalHandler(int sigNum);

#endif
