#include "Server.h"

using namespace std;

int main()
{
    SocketInfo *lpSockInfo;
    int argument = 1;

    struct sigaction sigAct;
    struct sigaction oldSigAct;

    sigAct.sa_handler = SignalHandler;
    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_flags = 0;

    sigaction(SIGINT, &sigAct, &oldSigAct);

    if((lpSockInfo = InitServer()) == NULL)
    {
        return -1;
    }

    if(setsockopt (lpSockInfo->Listen, SOL_SOCKET, SO_REUSEADDR, &argument, sizeof(argument)) == -1)
    {
        cerr << "Socket option failed" << endl;
        delete lpSockInfo->Address;
        delete lpSockInfo;
        return -1;
    }

    if(bind(lpSockInfo->Listen, (struct sockaddr *)&lpSockInfo->ServerAddr, sizeof(lpSockInfo->ServerAddr)) == -1)
    {
        cerr << "Bind failed" << endl;
        delete lpSockInfo->Address;
        delete lpSockInfo;
        return -1;
    }

    if(listen(lpSockInfo->Listen, 5) == -1)
    {
        cerr << "Listen failed" << endl;
        delete lpSockInfo->Address;
        delete lpSockInfo;
        return -1;
    }

    if(ServerLoop(lpSockInfo) < 0)
    {
        delete lpSockInfo->Address;
        delete lpSockInfo;
        return -1;
    }

    CleanupSocket(lpSockInfo);

    sigaction(SIGINT, &oldSigAct, NULL);

    cout << "EXITTING" << endl;
    return 0;
}

SocketInfo *InitServer()
{
    SocketInfo *lpSockInfo = new SocketInfo;

    if((lpSockInfo->Listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cerr << "Listen socket failed" << endl;
        delete lpSockInfo;
        return NULL;
    }
    lpSockInfo->Address = new char[32];

    strcpy(lpSockInfo->Address, "127.0.0.1");
    lpSockInfo->Port = DEFAULT_PORT;

    bzero((char *)&lpSockInfo->ServerAddr, sizeof(struct sockaddr_in));
    lpSockInfo->ServerAddr.sin_family = AF_INET;
    lpSockInfo->ServerAddr.sin_port = htons(DEFAULT_PORT);
    lpSockInfo->ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

    for(int i = 0; i < FD_SETSIZE; i++)
    {
        lpSockInfo->ClientList[i] = -1;
    }

    return lpSockInfo;
}

int ServerLoop(SocketInfo *lpSockInfo)
{
    char buffer[BUFF_LEN];
    char *bufferPtr;
    ssize_t bytesRecv;
    unsigned int clientLen;
    int maxFileDes;
    int maxIndex;
    int numReady;
    int i;
    int newSocket, fromClient;
    fd_set allDes, tempDes;

    maxFileDes = lpSockInfo->Listen;
    maxIndex = -1;

	FD_ZERO(&allDes);
	FD_SET(lpSockInfo->Listen, &allDes);

    while(true)
    {
        tempDes = allDes;
        numReady = select(maxFileDes + 1, &tempDes, NULL, NULL, NULL);

        // Check if a new client has connected
        if(FD_ISSET(lpSockInfo->Listen, &tempDes))
        {
            clientLen = sizeof(lpSockInfo->ClientAddr);
            if((newSocket = accept(lpSockInfo->Listen, (struct sockaddr *)&lpSockInfo->ClientAddr, &clientLen)) == -1)
            {
                cerr << "Accept failed" << endl;
                return -1;
            }

            cout << "Client connected: " << inet_ntoa(lpSockInfo->ClientAddr.sin_addr) << endl;

            for(i = 0; i < FD_SETSIZE; i++)
            {
                if(lpSockInfo->ClientList[i] < 0)
                {
                    lpSockInfo->ClientList[i] = newSocket;

                    break;
                }
            }
            if(i == FD_SETSIZE)
            {
                cerr << "Too many clients" << endl;
                return -1;
            }

            FD_SET(newSocket, &allDes);
            if(newSocket > maxFileDes)
            {
                maxFileDes = newSocket;
            }
            if(i > maxIndex)
            {
                maxIndex = i;
            }
            if(--numReady <= 0)
            {
                continue;
            }
        }

        // Check if a client has sent data
        for(i = 0; i < maxIndex; i++)
        {
            if((fromClient = lpSockInfo->ClientList[i]) < 0)
            {
                continue;
            }

            if(FD_ISSET(fromClient, &tempDes))
            {
                bufferPtr = buffer;
                while((bytesRecv = read(fromClient, bufferPtr, BUFF_LEN)) > 0)
                {
                    bufferPtr += bytesRecv;
                }
                for(int j = 0; j < maxIndex; j++)
                {
                    if(lpSockInfo->ClientList[j] == lpSockInfo->Listen || lpSockInfo->ClientList[j] == fromClient)
                    {
                        continue;
                    }

                    write(lpSockInfo->ClientList[j], buffer, BUFF_LEN);
                }
                if(bytesRecv == 0)
                {
                    cout << "Remote Address: " << inet_ntoa(lpSockInfo->ClientAddr.sin_addr) << " closed connection" << endl;
                	close(fromClient);
                	FD_CLR(fromClient, &allDes);
                    lpSockInfo->ClientList[i] = -1;
                }
                if(--numReady <= 0)
                {
                    continue;
                }
            }
        }
    }

    return 0;
}

int CleanupSocket(SocketInfo *lpSockInfo)
{
    cout << "Before" << endl;
    close(lpSockInfo->Listen);
    cout << "After" << endl;

    delete lpSockInfo->Address;
    delete lpSockInfo;

    return 0;
}

void SignalHandler(int sigNum)
{
    cout << "Signal caught" << endl;
}
