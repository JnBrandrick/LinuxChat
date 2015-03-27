/*----------------------------------------------------------------------------------------------------------------------
-- PROGRAM: Client - The client half of a Linux-based chat application.
--
-- SOURCE FILE: Client.cpp - Initializes and runs a TCP chat client.
--
-- FUNCTIONS:
--  int main                (void)
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
#include <pthread.h>
#include <curses.h>

#include "Client.h"
#include "NetworkHelper.h"

using namespace std;

string outputText;
string fileName;
bool serverFlag;

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
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    stringstream str;
    string address;
    int port = DEFAULT_PORT;
    int clientSocket;

    outputText = "";
    fileName = "";

    switch(argc)
    {
        case 2:
            address = string(argv[1]);
        break;
        case 3:
            address = string(argv[1]);
            port = atoi(argv[2]);
        break;
        case 4:
            address = string(argv[1]);
            port = atoi(argv[2]);
            fileName = string(argv[3]);
        break;
        default:
            DisplayError("Usage: ./Client <address> <port> <filename>");
            return -1;
    }

    if((clientSocket = InitClient(address, port)) < 0)
        return -1;

    serverFlag = true;

    MessageLoop(clientSocket);

    CleanupSocket(clientSocket);

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

    printw(TITLE_BANNER.c_str());

    pthread_create(&recvThread, NULL, ReceiveLoop, &clientSocket);

    while(serverFlag)
    {
        printw("You: ");

        while(input != '\n' && serverFlag)
        {
            input = getch();
            printw("%c", input);
            outputText += input;
        }
        outputText.pop_back();

        write(clientSocket, outputText.c_str(), strlen(outputText.c_str()) + 1);

        outputText = "";
        input = ' ';
    }

    EndCurses();

    DisplayError("Server disconnected");
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

        message = "\r" + string(buffer) + "          \nYou: " + outputText;

        printw("%s", message.c_str());
        refresh();

        if(fileName != "")
        {
            outputFile << message;
        }
    }
    while(bytesRecv > 0);

    outputFile.close();
    serverFlag = false;

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
