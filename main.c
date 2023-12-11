

#include <curses.h>


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ncurses.h>



static int _main(int argc, char** argv) {

    if(!has_colors()) {
        printw("Terminal does not support color");
        getch();
        return 1;
    }

    noecho();
    start_color();

    // define custom colors if supported
    if(can_change_color()) {
        init_color(COLOR_CYAN, 1, 100, 0);
    }

    int pair_id = 1;
    init_pair(pair_id, COLOR_CYAN, COLOR_WHITE);
    
    attron(COLOR_PAIR(pair_id));
    printw("this is some text");
    attroff(COLOR_PAIR(pair_id));


    getch();

    return 0;
}



int main(int argc, char** argv){

    int status;

    // init ncurses
    initscr();

    status = _main(argc, argv);

    endwin();


    return status;
}