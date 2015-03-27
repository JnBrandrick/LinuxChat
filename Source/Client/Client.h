/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: Client.h - Contains the definitions of the declarations in Client.cpp.
--
-- FUNCTIONS:
--  int InitClient  (string address, int prot)
--  int MessageLoop (int clientSocket)
--  int ReceiveLoop (void *param)
--  int StartCurses (void)
--  int EndCurses   (void)
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- NOTES:
--  This file contains all of the constants and function prototypes for Client.cpp.
----------------------------------------------------------------------------------------------------------------------*/
#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#define DEFAULT_PORT 7000
#define BUFF_LEN 256

const std::string TITLE_BANNER = "======================\n"
                                 "| Linux Chat Program |\n"
                                 "======================\n";


int InitClient(std::string address, int port);
void MessageLoop(int clientSocket);
void* ReceiveLoop(void* param);
void StartCurses();
void EndCurses();

#endif
