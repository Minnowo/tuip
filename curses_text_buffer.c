


#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"
#include "curses_text_buffer.h"


text_buffer_t* tb_malloc(const size_t length) {

    text_buffer_t* buf = malloc(sizeof(text_buffer_t));

    if(!buf) {
        return NULL;
    }

    #define UTF8_DECODE_PAD 3

    buf->buffer = malloc(UTF8_DECODE_PAD + length * sizeof(buf->buffer[0]));

    if(!buf->buffer) {
        free(buf);
        return NULL;
    }

    memset(buf->buffer, 0, length);

    buf->length = length;
    buf->cursor_pos = 0;
    buf->cursor_pos_utf8 = 0;
    buf->end_pos = 0;

    return buf;
}

void tb_free(text_buffer_t *buf) {

    if (!buf) {
        return;
    }

    if (buf->buffer) {
        free(buf->buffer);
    }

    free(buf);
}

bool tb_add_at_cur_ch(text_buffer_t *buf, const tbchar_t c) {

    if(!buf || buf->end_pos == buf->length - 1) {
        return 0;
    }

    if (buf->cursor_pos < buf->end_pos) {
        memmove(buf->buffer + buf->cursor_pos + 1,
                buf->buffer + buf->cursor_pos, buf->end_pos - buf->cursor_pos);
    }

    buf->buffer[buf->cursor_pos] = c;

    buf->cursor_pos++;
    buf->end_pos++;
    buf->buffer[buf->end_pos] = '\0';

    return 1;
}

bool tb_remove_at_cur_ch(text_buffer_t *buf) {

    if (!buf || buf->cursor_pos == 0) {
        return 0;
    }

    if (buf->cursor_pos < buf->end_pos) {
        memmove(buf->buffer + buf->cursor_pos - 1,
                buf->buffer + buf->cursor_pos, buf->end_pos - buf->cursor_pos);
    }

    buf->cursor_pos--;
    buf->end_pos--;
    buf->buffer[buf->end_pos] = '\0';

    return 1;
}


bool tb_set_contents(text_buffer_t *buf, const tbchar_t*text, size_t text_length) {

    if(!buf || !text) {
        return 0;
    }

    if(text_length > buf->length) {
        text_length = buf->length;
    }

    size_t i;
    size_t char_c = 0;

    for (i = 0; i < text_length && text[i] != '\0'; i++) {

        buf->buffer[i] = text[i];

    /*
        char_c++;
        uint32_t c = utf8_byte_count(text+i);

        if(c == 0 || c == 1) {
            buf->buffer[i] = text[i];
            continue;
        }

        if(i + c >= text_length) {
            break;
        }

        for(int j = 0; j < c; j++) {

            buf->buffer[i] = text[i];
            i++;
        }
        i--;
    */
    }

    buf->cursor_pos = i;
    buf->cursor_pos_utf8 = char_c;
    buf->end_pos = i;
    buf->buffer[i] = '\0';

    return 1;
}

void tb_move_cur_right(text_buffer_t *buf) {
    if (!buf || buf->cursor_pos >= buf->end_pos) {
        return;
    }

    buf->cursor_pos++;
}

void tb_move_cur_right_word(text_buffer_t *buf) {
    if (!buf) {
        return;
    }

    while (buf->cursor_pos != buf->end_pos &&
           isspace(buf->buffer[buf->cursor_pos]))
        buf->cursor_pos++;

    while (buf->cursor_pos != buf->end_pos &&
           !isspace(buf->buffer[buf->cursor_pos]))
        buf->cursor_pos++;
}

void tb_move_cur_left(text_buffer_t* buf) {
    if (!buf || buf->cursor_pos <= 0) {
        return;
    }

    buf->cursor_pos--;
}

void tb_move_cur_left_word(text_buffer_t* buf) {
    if (!buf) {
        return;
    }

    while (buf->cursor_pos != 0 &&
           isspace(buf->buffer[buf->cursor_pos]))
        buf->cursor_pos--;

    while (buf->cursor_pos != 0 &&
           !isspace(buf->buffer[buf->cursor_pos]))
        buf->cursor_pos--;
}

