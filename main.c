

#include <curses.h>


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ncurses.h>



int main(){

    // init ncurses
    initscr();


    // moves the cursor to a location on screen
    move(10, 5);

    printw("Hello world\n");

    // refresh the screen to match whats in memory
    refresh();

    // get user input, returns this key
    int key = getch();


    // print in top left
    move(0, 0);
    printw("got key %c with code %d\n", key, key);


    // move and print in 1 go
    mvprintw(5, 5, "This is some more text");

    getch();

    // clear the screen
    clear();
    getch();

    // end ncurses
    endwin();


    return 0;
}