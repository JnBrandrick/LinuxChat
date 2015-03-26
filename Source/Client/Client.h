#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#define DEFAULT_PORT 7000
#define BUFF_LEN 256

const std::string TITLE_BANNER = "======================\n"
                                 "| Linux Chat Program |\n"
                                 "======================\n";


int InitClient(std::string address, int port);
int MessageLoop(int clientSocket);
void* ReceiveLoop(void* param);
void StartCurses();
void EndCurses();

#endif
