#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#include <pthread.h>
#include <unistd.h>
#include <curses.h>

using namespace std;

int main()
{
    vector<int> num;

    num.push_back(0);
    num.push_back(1);
    num.push_back(2);
    num.push_back(3);
    num.push_back(4);

    for(unsigned int i = 0; i < num.size(); i++)
    {
        cout << i << " ; " << num.at(i) << endl;
    }
    cout << endl;

    num.erase(num.begin() + 2);

    for(unsigned int i = 0; i < num.size(); i++)
    {
        cout << i << " ; " << num.at(i) << endl;
    }

    return 0;
}

//void *output(void *param);
//
//string text;
//
//int main()
//{
//    pthread_t thread;
//    int ch = 0;
//    int row, col;
//
//    /* Basic initialization of curses lib */
//    initscr();
//    cbreak();
//    noecho(); /* Set this for interactive programs. */
//    intrflush(stdscr, FALSE);
//    scrollok(stdscr,TRUE);
//
//    pthread_create(&thread, NULL, output, NULL);
//
//    /* Ready to rumble! */
//    printw("Type a key. Use 'q' to quit\n");
//    while (ch != '\n')
//    {
//        ch = getch();
//        printw("%c", ch);
//
//        text += ch;
//    }
//
//    /* Cleanup time */
//    endwin();
//
//    cout << text << endl;
//
//    return 0;
//}
//
//void *output(void *param)
//{
//    while(true)
//    {
//        sleep(2);
//        printw("\rPLACEHOLDER\n%s", text.c_str());
//
//        refresh();
//    }
//
//    return NULL;
//}
