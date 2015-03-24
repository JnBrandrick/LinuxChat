#include "Client.h"

int main(int argc, char **argv)
{
    char *address;
    int port;

    switch(argc)
    {
        case 2:
            address = argv[1];
            port = DEFAULT_PORT;
        break;
        case 3:
            address = argv[1];
            port = argv[2];
        break;
        default:
            cerr << "Usage: ./Client <address> <port>" << endl;
            return -1;
    }

    

    return 0;
}
