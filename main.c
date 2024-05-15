
#include <ncurses.h>

#include <locale.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define FALLTHROUGH 
#define white_black 1
#define black_white 2
#define OUTPUT_FILE "/tmp/tuip"


/**
 * malloc n bytes or exit
 */
void* emalloc(size_t n) {

    void* m = malloc(n);

    if(m == NULL) {

        fprintf(stderr, "%s\n", strerror(errno));

        exit(1);
    }

    return m;
}


/**
 * duplicate the given string or exit
 */
char* estrdup(const char *s)
{
    size_t n = strlen(s);

    char *d = emalloc(n + 1);

    memcpy(d, s, n);

    d[n] = 0;

    return d;
}


/**
 * write the given string to the output file
 */
void write_output_str(const char* s) {

    FILE *fp = fopen(OUTPUT_FILE, "w");

    if (fp != NULL)
    {
        fputs(s, fp);

        fclose(fp);
    }
}


/**
 * draw a line on the window using the given color
 */
void draw_line(WINDOW* win, int attr, size_t w, size_t y, const char* text) {

    wattron(win, COLOR_PAIR(attr));
    for (int i = 0; i < w; i++) mvwaddch(win, y, i + 1, ' ');
    mvwaddstr(win, y, 1, text);
    wattroff(win, COLOR_PAIR(attr));
}


/**
 * the main loop
 */
int _main(int argc, char **argv) {

    if (argc <= 1) {

        printw("There was no input list!");

        getch();

        return 1;
    }

    if (!has_colors()) {

        printw("Terminal does not support color");

        getch();

        return 1;
    }

    // copy the start arguments to show in the list
    argc--;
    argv++;

    char** list_items = emalloc(argc * sizeof(char*));

    for (int i = 0; i < argc; i++)
        list_items[i] = estrdup(argv[i]);


    int exit_status = 1;

    // setup the ncurses window
    const int TOP_MARGIN = 2;

    int width = getmaxx(stdscr);
    int height = getmaxy(stdscr);
    int hheight = height / 2;

    int pos = 0;
    int scroll = 0;

    // create the subwindow
    WINDOW* win = subwin(stdscr, height, width, 0, 0);

    noecho();
    start_color();	
    curs_set(0);
    keypad(stdscr, TRUE);

    init_pair(white_black, COLOR_WHITE, COLOR_BLACK);
    init_pair(black_white, COLOR_BLACK, COLOR_WHITE);

    for(;;) {

        // ensure window sizes are taken into account
        width = getmaxx(stdscr);
        height = getmaxy(stdscr);
        hheight = height / 2;

        // draw the current index of the items
        mvwprintw(win, 1, 1, "Index: %d   Scroll: %d\n", pos, scroll);

        // draw the list of items
        for (int j = 0; j < height; j++) {

            int i = scroll + j;

            int attr = (i != pos) ? white_black : black_white;

            if (i >= argc) {

                draw_line(win, attr, width, j + TOP_MARGIN, "");

                continue;
            }

            draw_line(win, attr, width, j + TOP_MARGIN, list_items[i]);
        }

        // draw the white box outline
        box(win, 0, 0);

        // update the screen
        wrefresh(win);


        // handle user input
        int c = wgetch(win);

        switch (c) {

            case '\n':
            case KEY_ENTER:

                // the user has chosen something!
                write_output_str(list_items[pos]);
                exit_status = 0;

                goto done;

            case 'q':
            case 'Q':
                goto done;


            case 'g':

                pos = 0;

                FALLTHROUGH;

            case 'k':
            case 'K':
            case KEY_UP:

                pos--;

                if(pos < 0) {
                    pos = 0;
                }

                if(pos > hheight) {
                    scroll = pos - hheight;
                } else {
                    scroll = 0;
                }

                break;

            case 'G':

                pos = argc - 1;

                FALLTHROUGH;

            case 'j':
            case 'J':
            case KEY_DOWN:

                pos++;

                if(pos >= argc) {
                    pos = argc - 1;
                }

                if(pos > hheight) {
                    scroll = pos - hheight;
                } else {
                    scroll = 0;
                }

                break;
        }

    }

done:

    // cleanup
    delwin(win);

    for(int i = 0; i < argc; i++) {
        free(list_items[i]);
    }

    free(list_items);

    return exit_status;
}


int main(int argc, char **argv) {

    int status;

    // fixes broken utf8 and unicode stuff
    setlocale(LC_ALL, "");

    initscr();

    status = _main(argc, argv);

    endwin();

    return status;
}

