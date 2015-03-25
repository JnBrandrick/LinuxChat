#include <iostream>
#include <string>

#include <pthread.h>
#include <unistd.h>

using namespace std;

void *output(void *notUsed);

int main()
{
    string buffer;
    pthread_t thread;

    cout << "Enter Text:" << endl;

    pthread_create (&thread, NULL, output, NULL);

    cin >> buffer;

    cout << buffer << endl;

    return 0;
}

void* output(void* notUsed)
{
    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        cout << "THIS IS A THREAD, YAY!!" << endl;
    }

    return NULL;
}
