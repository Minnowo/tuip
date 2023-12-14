


#ifndef CURSES_TEXT_BUFFER_H
#define CURSES_TEXT_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef char tbchar_t;

/*
   This struct holds data for a text box
*/
struct text_buffer {

    // the length of buffer
    size_t length;

    // the cursor position, ascii
    size_t cursor_pos;

    // the cursor position, when utf
    size_t cursor_pos_utf8;

    // the end of text added
    size_t end_pos;

    // the text contents
    tbchar_t* buffer;
};

typedef struct text_buffer text_buffer_t;

text_buffer_t *tb_malloc(const size_t length);

void tb_free(text_buffer_t *buf);

/*
adds the given char to the text buffer at the current cursor position
*/
bool tb_add_at_cur_ch(text_buffer_t *buf, const tbchar_t c);

/*
removes the char at the cursor position
*/
bool tb_remove_at_cur_ch(text_buffer_t *buf);

/*
set the buffer contents to n bytes from the given string
*/
bool tb_set_contents(text_buffer_t* buf, const tbchar_t* text, size_t n);

void tb_move_cur_right(text_buffer_t *buf);

void tb_move_cur_right_word(text_buffer_t *buf);

void tb_move_cur_left(text_buffer_t *buf);

void tb_move_cur_left_word(text_buffer_t *buf);

#endif