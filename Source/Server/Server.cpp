#include <cstring>

#include "Server.h"

using namespace std;

int main()
{
    int listenSocket;
    int result = 0;

    if((listenSocket = InitServer()) < 0)
        return -1;

    if(ServerLoop(listenSocket) < 0)
    {
        result = -1;
    }

    CleanupSocket(listenSocket);

    return result;
}

int InitServer()
{
    int socket;

    if((socket = TCPSocket()) < 0)
        return -1;

    if(SetReuseOpt(socket) < 0)
        return -1;

    if(BindSocket(socket) < 0)
        return -1;

    if(ListenClient(socket) < 0)
        return -1;

    return socket;
}

int ServerLoop(int listenSocket)
{
    vector<ClientInfo> clientList;
    ClientInfo newClient;
    string address;
    int maxFileDes;
    int numReady;
    fd_set allDes, tempDes;

    maxFileDes = listenSocket;

	FD_ZERO(&allDes);
	FD_SET(listenSocket, &allDes);

    while(true)
    {
        tempDes = allDes;
        numReady = select(maxFileDes + 1, &tempDes, NULL, NULL, NULL);

        // Check if a new client has connected
        if(FD_ISSET(listenSocket, &tempDes))
        {
            newClient = AcceptClient(listenSocket);
            if(newClient.Socket < 0)
                return -1;

            DisplayOutput("Client connected: " + newClient.Address);

            clientList.push_back(newClient);

            if(clientList.size() == FD_SETSIZE)
            {
                DisplayError("Too many clients");
                return -1;
            }

            FD_SET(newClient.Socket, &allDes);
            if(newClient.Socket > maxFileDes)
            {
                maxFileDes = newClient.Socket;
            }
            if(--numReady <= 0)
            {
                continue;
            }
        }

        // Check if a client has sent data
        RecvClientMessage(clientList, &allDes, &tempDes, numReady);
    }

    return 0;
}

int RecvClientMessage(vector<ClientInfo> &clientList, fd_set *allDes, fd_set *tempDes, int numReady)
{
    ClientInfo fromClient;
    string message;
    char buffer[FD_SETSIZE];
    char *bufferPtr;
    int bytesRecv;

    for(unsigned int i = 0; i < clientList.size(); i++)
    {
        fromClient = clientList.at(i);
        if(fromClient.Socket < 0)
        {
            continue;
        }

        if(FD_ISSET(fromClient.Socket, tempDes))
        {
            bufferPtr = buffer;
            bytesRecv = read(fromClient.Socket, bufferPtr, BUFF_LEN);
            buffer[strlen(buffer)] = 0;

            message = fromClient.Address + ": " + string(buffer);

            if(bytesRecv == 0)
            {
                DisplayOutput("Remote Address: " + fromClient.Address + " closed connection");
                close(fromClient.Socket);

                FD_CLR(fromClient.Socket, allDes);
                clientList.erase(clientList.begin() + i);
                i--;
                continue;
            }
            DisplayOutput(message);
            for(unsigned int j = 0; j < clientList.size(); j++)
            {
                if(clientList.at(j).Socket == fromClient.Socket)
                {
                    continue;
                }

                write(clientList.at(j).Socket, message.c_str(), strlen(message.c_str()) + 1);
            }
            strcpy(buffer, "");
            bytesRecv = 0;
            if(--numReady <= 0)
            {
                break;
            }
        }
    }

    return 0;
}
