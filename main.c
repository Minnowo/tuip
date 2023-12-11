

#include <curses.h>


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ncurses.h>



int main(){

    // init ncurses
    initscr();


    int h, w, y, x;
    h = 10;
    w = h * 2;
    y = x = 0;

    // creating a new window
    WINDOW* win = newwin(h, w, y, x);

    // we need to refresh the screen for the window
    refresh();

    // outline the window in a box
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "This is a box");

    // refresh only our window
    wrefresh(win);

    getch();

    // end ncurses
    endwin();


    return 0;
}