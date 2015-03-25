#include "Client.h"

using namespace std;

int main(int argc, char **argv)
{
    SocketInfo *lpSockInfo;
    char *address = new char[32];
    int port;
    stringstream str;

    switch(argc)
    {
        case 2:
            address = argv[1];
            port = DEFAULT_PORT;
        break;
        case 3:
            address = argv[1];
            str << argv[2];
            if(!(str >> port))
            {
                cerr << "Port must be a number" << endl;
                return -1;
            }
        break;
        default:
            cerr << "Usage: ./Client <address> <port>" << endl;
            return -1;
    }

    if((lpSockInfo = InitClient(address, port)) == NULL)
    {
        return -1;
    }

    cout << TITLE_BANNER << endl;

    MessageLoop(lpSockInfo);

    CleanupSocket(lpSockInfo);

    delete[] address;
    return 0;
}

SocketInfo *InitClient(char *address, int port)
{
    SocketInfo *lpSockInfo = new SocketInfo;

    if((lpSockInfo->Socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cerr << "Socket failed" << endl;
        return NULL;
    }

    bzero((char *)&lpSockInfo->ServerAddr, sizeof(struct sockaddr_in));
    lpSockInfo->ServerAddr.sin_family = AF_INET;
    lpSockInfo->ServerAddr.sin_port = htons(port);
    if ((lpSockInfo->Host = gethostbyname(address)) == NULL)
    {
        cerr << "GetHostByName sailed" << endl;
        return NULL;
    }
    bcopy(lpSockInfo->Host->h_addr, (char *)&lpSockInfo->ServerAddr.sin_addr, lpSockInfo->Host->h_length);

    if (connect (lpSockInfo->Socket, (struct sockaddr *)&lpSockInfo->ServerAddr, sizeof(lpSockInfo->ServerAddr)) == -1)
	{
		cerr << "Connect failed" << endl;
		return NULL;
	}

    return lpSockInfo;
}

int MessageLoop(SocketInfo *lpSockInfo)
{
    pthread_t recvThread;
    pthread_create(&recvThread, NULL, ReceiveLoop, &lpSockInfo->Socket);

    while(true)
    {
        getline(cin, lpSockInfo->Output);

        write(lpSockInfo->Socket, lpSockInfo->Output.c_str(), strlen(lpSockInfo->Output.c_str()) + 1);
    }

    return 0;
}

void* ReceiveLoop(void* param)
{
    int socket = *(int *)param;
    char buffer[BUFF_LEN];
    cout << socket << endl;

    while(true)
    {
        read(socket, buffer, BUFF_LEN);
        buffer[strlen(buffer)] = 0;

        cout << "\r" << buffer << endl;
    }

    return NULL;
}

int CleanupSocket(SocketInfo *lpSockInfo)
{
    close(lpSockInfo->Socket);
    delete lpSockInfo;

    return 0;
}
