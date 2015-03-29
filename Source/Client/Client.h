/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: Client.h - Contains the definitions of the declarations in Client.cpp.
--
-- FUNCTIONS:
--  int InitClient      (string address, int prot)
--  int MessageLoop     (int clientSocket)
--  int ReceiveLoop     (void *param)
--  int StartCurses     (void)
--  int EndCurses       (void)
--  void SignalHandler  (int sigNum)
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
#define EOT 4
#define BACKSPACE 127
#define DELETE 8

const char NL = '\n';
const char CR = '\r';

std::string USAGE_COUNT = "No arguments given";
std::string USAGE_INVALID = "Invalid argument: ";
std::string USAGE_ADDR = "Argument -a needs an address";
std::string USAGE_ARGS = "Usage: ./Client -a <address> -p <port> -f <filename>";
const char* TITLE_BANNER = "======================\n"
                           "| Linux Chat Program |\n"
                           "======================\n";
const char* YOU = "You: ";
std::string DISCONNECT = "Disconnected from server";

int InitClient(std::string address, int port);
void MessageLoop(int clientSocket);
void* ReceiveLoop(void* param);
void StartCurses();
void EndCurses();
void SignalHandler(int sigNum);

#endif
