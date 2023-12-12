


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <curses.h>
#include <ncurses.h>
#include <form.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define MOD(a, b) ((((a) % (b)) + (b)) % (b))

#define KEY_ESCAPE 27
#define KEY_BACKSPACE_2 127

char* estrdup(const char *s)
{
	size_t n = strlen(s) + 1;

	char *d = malloc(n);

	if (d == NULL)
		fprintf(stderr, "Could not malloc sizeof(%zu)\n", n);

	memcpy(d, s, n);

	return d;
}


void draw_line(WINDOW* win, int attr, size_t w, size_t y, char* text) {

    wattron(win, COLOR_PAIR(attr));
    for (int i = 0; i < w; i++) mvwaddch(win, y, i + 1, ' ');
    mvwaddstr(win, y, 1, text);
    wattroff(win, COLOR_PAIR(attr));
}

int rename_dialog(WINDOW *win, int w_, int h_, int x_, int y_, char *to_rename, char** newname) {

    int w, h, x, y;

    w = w_ / 2;
    h = h_ / 2;
    x = w_ / 2 - w / 2;
    y = h_ / 2 - h / 2;

    WINDOW *win_form = derwin(win, h, w, y, x);
    keypad(win_form, TRUE);

    FIELD *field[2];

    field[0] = new_field(1,                                 // height
                         w - 5,                             // width
                         getbegy(win_form) + h / 2 + h / 4, // start y
                         getbegx(win_form) + 2,             // start x
                         0,                                 // offscreen rows
                         0                                  // offscreen cols
    );
    field[1] = NULL;

    // // Print a line for the option
    set_field_back(field[0], A_UNDERLINE);

    // // Don't go to next field when this
    field_opts_off(field[0], O_AUTOSKIP);

	set_field_opts(field[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_buffer(field[0], 0, to_rename);


    FORM *form = new_form(field);

    int rows, cols;
	scale_form(form, &rows, &cols);

    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, rows, cols, 2, 2));

    post_form(form);

    touchwin(win_form);
    noecho();

    int run = 1;
    int ret = 0;

    do {

        box(win_form, 0, 0);
        mvwprintw(win_form, 1, 1, "Rename Item");
        wrefresh(win_form);

        int ch = wgetch(win_form);

        switch (ch) {

            case KEY_ESCAPE:
                run = 0;
                ret = 0;
                break;

            case '\n':
            case KEY_ENTER:
                run = 0;
                ret = 1;

                form_driver(form, REQ_VALIDATION);

                char* r = strdup(field_buffer(field[0], 0));

                if (r != NULL) {
                    *newname = r;
                } else {
                    ret = 0;
                }

                break;

            case KEY_BACKSPACE:
            case KEY_BACKSPACE_2:
                form_driver(form, REQ_DEL_PREV);
                break;

            case KEY_DC:
                form_driver(form, REQ_DEL_CHAR);
                break;

            default:
                form_driver(form, ch);
                break;
        }

    } while (run);

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

    argc--;
    argv++;

    char** list_items = malloc(argc * sizeof(char*));

    for (int i = 0; i < argc; i++) {
        list_items[i] = estrdup(argv[i]);
    }

    int scroll_height = 15;
    int bottom_height = 5;

    WINDOW *win;
	int startx, starty, width, height;

    height = scroll_height + bottom_height;
    width = 100;
    starty = (LINES - height) / 2; /* Calculating for a center placement */
    startx = (COLS - width) / 2;   /* of the window		*/
    win = subwin(stdscr, height, width, starty, startx);

    noecho();
    start_color();	
    // nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	// halfdelay(5);


    #define white_black 1
    #define black_white 2
    init_pair(white_black, COLOR_WHITE, COLOR_BLACK);
    init_pair(black_white, COLOR_BLACK, COLOR_WHITE);


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

            draw_line(win, attr, width - 2, i + 2, list_items[i]);
        }

        box(win, 0, 0);
        wrefresh(win);

        int c = wgetch(win);
        char* newname;

        switch (c) {

        case '\n':
        case KEY_ENTER:

            if (rename_dialog(win, width, height, startx, starty + height,
                              list_items[pos], &newname)) {

                free(list_items[pos]);
                list_items[pos] = newname;
            }

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


    for(int i = 0; i < argc; i++) {
        free(list_items[i]);
    }

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