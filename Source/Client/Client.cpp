#include <iostream>
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
bool serverFlag;

int main(int argc, char **argv)
{
    stringstream str;
    string address;
    int port;
    int clientSocket;

    switch(argc)
    {
        case 2:
            address = string(argv[1]);
            port = DEFAULT_PORT;
        break;
        case 3:
            address = string(argv[1]);
            str << argv[2];
            if(!(str >> port))
            {
                DisplayError("Port must be a number");
                return -1;
            }
        break;
        default:
            DisplayError("Usage: ./Client <address> <port>");
            return -1;
    }

    if((clientSocket = InitClient(address, port)) < 0)
        return -1;

    serverFlag = true;

    MessageLoop(clientSocket);

    CleanupSocket(clientSocket);

    return 0;
}

int InitClient(string address, int port)
{
    int clientSocket;

    if((clientSocket = TCPSocket()) < 0)
        return -1;

    if(ServerConnect(clientSocket, address, port) < 0)
        return -1;

    return clientSocket;
}

int MessageLoop(int clientSocket)
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

    return 0;
}

void* ReceiveLoop(void* param)
{
    int clientSocket = *(int *)param;
    int bytesRecv;
    char buffer[BUFF_LEN];

    do
    {
        bytesRecv = read(clientSocket, buffer, BUFF_LEN);
        buffer[strlen(buffer)] = 0;

        printw("\r%s\nYou: %s", buffer, outputText.c_str());
        refresh();
    }
    while(bytesRecv > 0);

    serverFlag = false;

    return NULL;
}

void StartCurses()
{
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    scrollok(stdscr,TRUE);
}

void EndCurses()
{
    endwin();
}
