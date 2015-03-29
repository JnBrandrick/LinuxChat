/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: Client.cpp - Initializes and runs a TCP chat client.
--
-- FUNCTIONS:
--  int main            (void)
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
--  This program makes heavy use of wapper functions held in the NetworkHelper.cpp file.
--
--  It sends messages to a multiplex TCP server. This server forwards the message to all other connected clients.
--
--  The Ncurses library is used to prvent orverwriting of user text.
----------------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <atomic>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <curses.h>

#include "Client.h"
#include "NetworkHelper.h"

using namespace std;

string clientText;
string fileName;
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
-- INTERFACE: int main (int argc, char **argv)
--
-- PARAMETERS:
--  argc    -> The number terminal arguments entered
--  **argv  -> The array of terminal arguments
--
-- RETURNS: int
--  0  -> Normal termination
-- -1  -> Abnormal termination
--
-- NOTES:
--  This function starts the Client program. It first validates the terminal arguments entered, then initializes
--      a TCP socket with the given address and port.
--  It then runs the chat functionality. After the user quits, the socket is cleaned up and the program exits.
--
--  Usage: <Program Name> <Server Address> <Server Port> <Optional File Name>
--  Default port is 7000
--
--  This function also disables the CTRL-C signal so it can be caught ad handled. It then enables it before returning.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    stringstream str;
    string address;
    int port = DEFAULT_PORT;
    int clientSocket;
    int optRet;
    char optString[] = "a:p:f:";

    struct sigaction sigAct;
    struct sigaction oldSigAct;

    clientText = "";
    fileName = "";

    if(argc == 1)
    {
        DisplayError(USAGE_COUNT);
        return -1;
    }

    while((optRet = getopt(argc, argv, optString)) != -1)
    {
        switch(optRet)
        {
            case 'a':
                address = string(optarg);
            break;
            case 'p':
                str << optarg;
                str >> port;
            break;
            case 'f':
                fileName = string(optarg);
            break;
            case '?':
                DisplayError(USAGE_ARGS);
            return -1;
        }
    }
    
    if(optind < argc)
    {
        DisplayError(USAGE_INVALID + string(argv[optind]));
        return -1;
    }

    sigAct.sa_handler = SignalHandler;
    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_flags = 0;

    sigaction(SIGINT, &sigAct, &oldSigAct);

    if((clientSocket = InitClient(address, port)) < 0)
        return -1;

    serverExitFlag = true;

    MessageLoop(clientSocket);

    CleanupSocket(clientSocket);
    
    sigaction(SIGINT, &oldSigAct, NULL);

    return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: InitClient
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int InitClient (string address, int port)
--
-- PARAMETERS:
--  address -> Address of the TCP server
--  port    -> Port of the TCP server
--
-- RETURNS: int
--  0  -> Successfully created TCP socket
-- -1  -> Error occurred while setting up TCP socket
--
-- NOTES:
--  This function creates a TCP socket and connects it to the remote multiplexed server.
----------------------------------------------------------------------------------------------------------------------*/
int InitClient(string address, int port)
{
    int clientSocket;

    if((clientSocket = TCPSocket()) < 0)
        return -1;

    if(ServerConnect(clientSocket, address, port) < 0)
        return -1;

    return clientSocket;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: MessageLoop
--
-- DATE: March 12, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int MessageLoop (int clientSocket)
--
-- PARAMETERS:
--  clientSocket    -> A TCP client socket
--
-- RETURNS: void
--
-- NOTES:
--  This function has three important functions:
--      - It starts and stops Ncurses
--      - It starts the receive thread
--      - It starts and stops the message loop
----------------------------------------------------------------------------------------------------------------------*/
void MessageLoop(int clientSocket)
{
    pthread_t recvThread;
    char input;

    StartCurses();

    printw(TITLE_BANNER);

    pthread_create(&recvThread, NULL, ReceiveLoop, &clientSocket);

    while(serverExitFlag)
    {
        printw(YOU);
        clientText = "";

        while(input != NL && serverExitFlag)
        {
            input = getch();
            if(isprint(input))
            {
                clientText.push_back(input);
                printw("%c", input);
            }
            if((input == BACKSPACE || input == DELETE) && clientText.length() > 0)
            {
                clientText.pop_back();
                printw("%c", CR);
                printw("%*s", clientText.length() + 10, " ");
                printw("%c%s%s", CR, YOU, clientText.c_str());
            }
            if(input == NL)
                printw("\n");
        }

        write(clientSocket, clientText.c_str(), strlen(clientText.c_str()) + 1);

        input = CR;
    }

    EndCurses();

    DisplayError(DISCONNECT);
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ReceiveLoop
--
-- DATE: March 19, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: int ReceiveLoop (void *param)
--
-- PARAMETERS:
--  *param  -> A void pointer which contains the TCp client socket for the program
--
-- RETURNS: void*
--  NULL -> Return is not used
--
-- NOTES:
--  This function is run in the receive thread.
--
--  It reads incoming messages from the server and displays them to the screen.
--
--  In order to prevent the overwriting of the user's input, the input is written again on the next line.
--
--  All received messages are outoutted to a file.
----------------------------------------------------------------------------------------------------------------------*/
void* ReceiveLoop(void* param)
{
    string serverText;
    string message;
    int clientSocket = *(int *)param;
    int bytesRecv;
    char buffer[BUFF_LEN];
    ofstream outputFile;
    
    outputFile.open(fileName);

    do
    {
        bytesRecv = read(clientSocket, buffer, BUFF_LEN);
        buffer[strlen(buffer)] = 0;

        serverText = string(buffer);
        
        message = CR + serverText + NL + YOU + clientText;
        
        printw("%s", message.c_str());
        refresh();
        
        if(fileName != "")
            outputFile << serverText << endl;
    }
    while(buffer[0] != EOT && bytesRecv > 0 && serverExitFlag);

    serverExitFlag = false;

    return NULL;
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: StartCurses
--
-- DATE: March 20, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: void StartCurses (void)
--
-- RETURNS: void
--
-- NOTES:
--  This function starts Ncurses and sets several options to allow for user interactivity.
----------------------------------------------------------------------------------------------------------------------*/
void StartCurses()
{
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    scrollok(stdscr,TRUE);
}

/*----------------------------------------------------------------------------------------------------------------------
-- FUNCTION: EndCurses
--
-- DATE: March 20, 2015
--
-- DESIGNER: Julian Brandrick
--
-- PROGRAMMER: Julian Brandrick
--
-- INTERFACE: void EndCurses (void)
--
-- RETURNS: void
--
-- NOTES:
--  This function ends and cleans up Ncurses.
----------------------------------------------------------------------------------------------------------------------*/
void EndCurses()
{
    endwin();
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
--  This function catches a signal and tells the client to disconnect and exit.
----------------------------------------------------------------------------------------------------------------------*/
void SignalHandler(int sigNum)
{
    serverExitFlag = false;
}