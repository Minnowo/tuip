

#include <curses.h>


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ncurses.h>



int main(){

    // init ncurses
    initscr();

    printw("Hello world\n");

    // refresh the screen to match whats in memory
    refresh();

    // get user input, returns this key
    int key = getch();

    printw("got key %c with code %d\n", key, key);

    getch();

    // end ncurses
    endwin();


    return 0;
}