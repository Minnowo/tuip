


// https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <ctype.h>
#include <curses.h>
#include <locale.h>
#include <ncurses.h>
#include <form.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "curses_text_buffer.h"
#include "utf8.h"


#define MOD(a, b) ((((a) % (b)) + (b)) % (b))

#define KEY_ESCAPE 27
#define KEY_BACKSPACE_2 127

#define NCURSES_CTRL_MASK 0x1F

#define white_black 1
#define black_white 2

char* estrdup(const char *s)
{
	size_t n = strlen(s) + 1;

	char *d = malloc(n);

	if (d == NULL)
		fprintf(stderr, "Could not malloc sizeof(%zu)\n", n);

	memcpy(d, s, n);

	return d;
}

static char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

bool rename_file(const char *path, const char *newpath) {

        if (access(path, R_OK) != 0) {

            fprintf(
                stderr,
                "The file %s does not exist or is missing read permission\n",
                path);

            return false;
        }

        return rename(path, newpath) == 0;
}

void draw_line(WINDOW* win, int attr, size_t w, size_t y, char* text) {

    wattron(win, COLOR_PAIR(attr));
    for (int i = 0; i < w; i++) mvwaddch(win, y, i + 1, ' ');
    mvwaddstr(win, y, 1, text);
    wattroff(win, COLOR_PAIR(attr));
}

int rename_dialog(WINDOW *win, int w_, int h_, int x_, int y_, char *to_rename,
                  char **newname) {

    int w, h, x, y;

    w = w_ / 2;
    h = h_ / 2;
    x = w_ / 2 - w / 2;
    y = h_ / 2 - h / 2;

    WINDOW *win_form = derwin(win, h, w, y, x);
    keypad(win_form, TRUE);

    int tw, th, tx, ty;

    tw = w - 5;
    th = 1;
    tx = 2;
    ty = h / 2;

    touchwin(win_form);

    int run = 1;
    int ret = 0;

    size_t len = strlen(to_rename);

    text_buffer_t *text_buf = tb_malloc(tw + 1);

    tb_set_contents(text_buf, to_rename, len);

    do {

            box(win_form, 0, 0);
            mvwprintw(win_form, 1, 1, "Rename Item");


        /*
            for (int i = 0, c = 0; i < text_buf->end_pos;) {

                uint32_t utf8 = utf8_byte_count(text_buf->buffer + i);
                c++;

                if(utf8 == 0) {
                    i++;
                    continue;
                }

                wmove(win_form, ty, tx+i);

                if (i == text_buf->cursor_pos || c == text_buf->cursor_pos_utf8) {
                    wattron(win_form, COLOR_PAIR(black_white));
                    waddnstr(win_form, text_buf->buffer + i, utf8);
                    wattroff(win_form, COLOR_PAIR(black_white));
                }
                else {
                    waddnstr(win_form, text_buf->buffer + i, utf8);
                }

                i += utf8;
            }
        */

            for (int i = 0; i < text_buf->length; i++)
                mvwprintw(win_form, ty, tx + i, " ");
            mvwprintw(win_form, ty, tx, "%s", text_buf->buffer);
            wmove(win_form, ty, tx + text_buf->cursor_pos);

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

                char* r = trim_whitespaces(text_buf->buffer);

                if (r != NULL && *r != '\0') {
                    *newname = strdup(r);
                } else {
                    ret = 0;
                }

                break;

            case 'h' & NCURSES_CTRL_MASK:
                tb_move_cur_left_word(text_buf);
                break;
            case KEY_LEFT:
                tb_move_cur_left(text_buf);
                break;

            case 'l' & NCURSES_CTRL_MASK:
                tb_move_cur_right_word(text_buf);
                break;
            case KEY_RIGHT:
                tb_move_cur_right(text_buf);
                break;

            case KEY_BACKSPACE:
            case KEY_BACKSPACE_2:
                tb_remove_at_cur_ch(text_buf);
                break;

            case KEY_DC:
                break;

            default:
                tb_add_at_cur_ch(text_buf, ch);
                break;
            }

    } while (run);

    tb_free(text_buf);

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

                if (rename_file(list_items[pos], newname)) {
                    free(list_items[pos]);
                    list_items[pos] = newname;
                }
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

    free(list_items);

    return 0;
}

int main(int argc, char **argv) {

    int status;

    setlocale(LC_ALL, "");

    // init ncurses
    initscr();

    status = _main(argc, argv);

    endwin();

    return status;
}