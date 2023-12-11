

#include <curses.h>
#include <form.h>

// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ncurses.h>
#include <stdio.h>


#define MOD(a, b) ((((a) % (b)) + (b)) % (b))



void draw_line(WINDOW* win, int attr, size_t w, size_t y, char* text) {

    wattron(win, COLOR_PAIR(attr));
    for (int i = 0; i < w; i++) mvwaddch(win, y, i + 1, ' ');
    mvwaddstr(win, y, 1, text);
    wattroff(win, COLOR_PAIR(attr));
}

int rename_dialog(WINDOW *win, int width, int height, char *to_rename) {

    WINDOW *win_form = derwin(win, 20, 78, 3, 1);
    FIELD *field[2];

    field[0] = new_field(1,          // height
                         width,      // width
                         height - 2, // start y
                         1,          // start x
                         0,          // offscreen rows
                         0           // offscreen cols
    );
    field[1] = NULL;

    // Print a line for the option
    set_field_back(field[0], A_UNDERLINE);

    // Don't go to next field when this
    field_opts_off(field[0], O_AUTOSKIP);

	set_field_opts(field[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

    FORM *form = new_form(field);
    post_form(form);

	refresh();

    box(win_form, 0, 0);

    int run = 1;
    int ret = 0;


    do {

        int ch = getch();
        switch (ch) {
            case '\n':
            case KEY_ENTER:
                run = 0;
                ret = 1;
                break;

            // Delete the char before cursor
            case KEY_BACKSPACE:
            case 127:
                form_driver(form, REQ_DEL_PREV);
                break;

            // Delete the char under the cursor
            case KEY_DC:
                form_driver(form, REQ_DEL_CHAR);
                break;

            case KEY_DOWN:
                /* Go to next field */
                form_driver(form, REQ_NEXT_FIELD);
                /* Go to the end of the present buffer */
                /* Leaves nicely at the last character */
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_UP:
                /* Go to previous field */
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            default:
                /* If this is a normal character, it gets */
                /* Printed				  */
                form_driver(form, ch);
                break;
        }

    } while (run);

        /* Un post form and free the memory */
	unpost_form(form);
	free_form(form);
	free_field(field[0]);

    delwin(win_form);
    return ret;
}

static int _main(int argc, char **argv) {

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


    int scroll_height = 15;
    int bottom_height = 5;

    WINDOW *win;
	int startx, starty, width, height;

    height = scroll_height + bottom_height;
    width = 100;
    starty = (LINES - height) / 2; /* Calculating for a center placement */
    startx = (COLS - width) / 2;   /* of the window		*/
    win = newwin(height, width, starty, startx);


    noecho();
    start_color();	
    nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	halfdelay(5);


    #define white_black 1
    #define black_white 2
    init_pair(white_black, COLOR_WHITE, COLOR_BLACK);
    init_pair(black_white, COLOR_BLACK, COLOR_WHITE);

    argc--;
    argv++;

    int pick = 1;
    int pos = 0;
    int scroll = 0;

    do {
        int rheight = scroll_height;

        wmove(win, 1, 1);
        wprintw(win, "Index: %d\n", pos);

        rheight--;

        for (int j = 0, i = scroll; j < rheight; j++) {

            i = scroll + j;

            if (i >= argc) {
                wprintw(win, "\n");
                continue;
            }
            int attr = white_black;

            if (i == pos) {
                attr = black_white;
            }

            draw_line(win, attr, width - 2, i + 2, argv[i]);
        }

        box(win, 0, 0);
        wrefresh(win);

        int c = wgetch(win);

        switch (c) {

        case '\n':
        case KEY_ENTER:

            rename_dialog(win, width, height, argv[pos]);

            break;

        case 'q':
        case 'Q':
            pick = 0;
            break;


        case 'k':
        case 'K':
        case KEY_UP:
            pos--;
            if(pos < 0) {
                pos = 0;
            }


            if(pos > argc - rheight / 2) {
            }
            else if(pos > rheight / 2) {
                scroll = pos - rheight/2;
            } else {
                scroll = 0;
            }

            // scroll = MOD(scroll - 1, height);
            break;

        case 'j':
        case 'J':
        case KEY_DOWN:
            pos++;
            if(pos >= argc) {
                pos = argc - 1;
            }

            if(pos > argc - rheight / 2) {
            }
            else if(pos > rheight / 2) {
                scroll = pos - rheight/2;
            } else {
                scroll = 0;
            }

            break;
        }

    } while (pick);

    delwin(win);

    return 0;
}

int main(int argc, char **argv) {

    int status;

    // init ncurses
    initscr();

    status = _main(argc, argv);

    endwin();

    return status;
}