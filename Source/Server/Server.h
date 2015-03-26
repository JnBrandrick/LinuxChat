#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "NetworkHelper.h"

#define BUFF_LEN 256

int InitServer();
int ServerLoop(int listenSocket);
int RecvClientMessage(std::vector<ClientInfo> &clientList, fd_set *allDes, fd_set *tempDes, int numReady);

#endif
