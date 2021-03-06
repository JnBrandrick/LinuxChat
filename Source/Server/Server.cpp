/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Server - The server half of a Linux-based chat application.
--
-- SOURCE FILE: Server.cpp - Initializes and runs a multiplex TCP chat server.
--
-- FUNCTIONS:
--  int main                (void)
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
--  This program makes heavy use of wapper functions held in the NetworkHelper.cpp file.
--
--  It receives messages from clients and forwards them to all other connected clients.
----------------------------------------------------------------------------------------------------------------------*/
#include <sstream>
#include <cstring>
#include <signal.h>

#include "Server.h"

using namespace std;

vector<ClientInfo> clientList;
bool serverExitFlag;

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int main (void)
--
-- RETURNS: int
--  0  -> Normal termination
-- -1  -> Abnormal termination
--
-- NOTES:
--  This function starts the Server program. It first initializes a listening TCP socket and runs the server loop.
--  When this loop finishes it closes the listening socket and exits.
--  This function also disables the CTRL-C signal so it can be caught ad handled. It then enables it before returning.
----------------------------------------------------------------------------------------------------------------------*/
int main()
{
    int listenSocket;
    int result = 0;

    struct sigaction sigAct;
    struct sigaction oldSigAct;

    sigAct.sa_handler = SignalHandler;
    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_flags = 0;
    
    serverExitFlag = true;

    sigaction(SIGINT, &sigAct, &oldSigAct);

    if((listenSocket = InitServer()) < 0)
        return -1;

    if(ServerLoop(listenSocket) < 0)
    {
        result = -1;
    }

    CleanupSocket(listenSocket, clientList);
    
    sigaction(SIGINT, &oldSigAct, NULL);

    return result;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: InitServer
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int InitServer (void)
--
-- RETURNS: int
--  0  -> Successfully created TCP socket
-- -1  -> Error occurred while setting up TCP socket
--
-- NOTES:
--  This function:
--      - creates a TCP socket
--      - sets it reuseable
--      - binds it to a port
--      - starts listening for incoming connections
----------------------------------------------------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ServerLoop
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int ServerLoop (int listenSocket)
--
-- RETURNS: int
--  0  -> Successfully ran through server execution
-- -1  -> Accepting a client failed
--     or Too many clients have been accepted
--
-- NOTES:
--  Holds all of the functionality of the server's multiplexing. This is done through use of the select function.
--  If an event occurred on the listening socket, then a new client is trying to connect.
--  If an event occurered on any of the other clients, then a client is sending a chat message.
----------------------------------------------------------------------------------------------------------------------*/
int ServerLoop(int listenSocket)
{
    ClientInfo newClient;
    string address;
    int maxFileDes;
    int numReady;
    fd_set allDes, tempDes;

    maxFileDes = listenSocket;

	FD_ZERO(&allDes);
	FD_SET(listenSocket, &allDes);

    while(serverExitFlag)
    {
        tempDes = allDes;
        numReady = select(maxFileDes + 1, &tempDes, NULL, NULL, NULL);

        // Check if a new client has connected
        if(FD_ISSET(listenSocket, &tempDes) && serverExitFlag)
        {
            newClient = AcceptClient(listenSocket);
            if(newClient.Socket < 0)
                return -1;

            newClient.Name = Count(newClient.Address);

            DisplayOutput("Client connected: " + newClient.Name);

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

        RecvClientMessage(&allDes, &tempDes, numReady);
    }
    
    DisplayOutput("Server disconnected");

    return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: RecvClientMessage
--
-- DATE: March 19, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int RecvClientMessage (fd_set *allDes, fd_set *tempDes, int numReady)
--
-- PARAMETERS:
--  *allDes     -> Pointer to the master set of socket file descriptors
--  *tempDes    -> Pointer to the temporary set of socket file descriptors
--  numReady    -> The number of sockets for which an event has occurred
--
-- RETURNS: void
--
-- NOTES:
--  This function:
--      - creates a TCP socket
--      - sets it reuseable
--      - binds it to a port
--      - starts listening for incoming connections
----------------------------------------------------------------------------------------------------------------------*/
void RecvClientMessage(fd_set *allDes, fd_set *tempDes, int numReady)
{
    ClientInfo fromClient;
    string message;
    char buffer[FD_SETSIZE];
    char *bufferPtr;
    int bytesRecv;

    for(unsigned int i = 0; i < clientList.size() && serverExitFlag; i++)
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

            message = fromClient.Name + ": " + string(buffer);

            if(bytesRecv == 0)
            {
                DisplayOutput("Client disconnected: " + fromClient.Name);
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

                write(clientList.at(j).Socket, message.c_str(), message.length() + 1);
            }

            // Need to clear the write buffer here
            //  Characters are being sent on disconnect

            strcpy(buffer, "");
            if(--numReady <= 0)
            {
                break;
            }
        }
    }
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Count
--
-- DATE: March 23, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: string Count (string address)
--
-- PARAMETERS:
--  address -> The address to be counted
--
-- RETURNS: string
--
-- NOTES:
--  This function counts the number of occurences of "address" in the client list;
--
--  It returns the name of the client, which is a concatenation of its address plus the number of clients with the
--      same address and separated by a dash. eg:
--      "192.168.1.51-1" -> The first client with the address "192.168.1.51".
----------------------------------------------------------------------------------------------------------------------*/
string Count(string address)
{
    stringstream str;
    int num = 1;
    
    for(unsigned int i = 0; i < clientList.size(); i++)
    {
        if(clientList.at(i).Address == address)
            num++;
    }
    
    str << address << "-" << num;
    
    return str.str();
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SignalHandler
--
-- DATE: March 23, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: void SignalHandler (int sigNum)
--
-- PARAMETERS:
--  sigNum  -> Unused
--
-- RETURNS: void
--
-- NOTES:
--  This function catches a signal and sends a shutdown message to all connected clients. It then tells the server
--      loop to exit.
----------------------------------------------------------------------------------------------------------------------*/
void SignalHandler(int sigNum)
{
    char exitChar = EOT;
    
    for(unsigned int i = 0; i < clientList.size(); i++)
    {
        write(clientList.at(i).Socket, &exitChar, sizeof(exitChar));
    }
    
    serverExitFlag = false;
}