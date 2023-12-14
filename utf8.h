#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define utf8_is_codep1(s) ((((s)[0] & 0x80) == 0x00))

#define utf8_is_codep2(s)                                                      \
    ((((s)[0] & 0xE0) == 0xC0) && (((s)[1] & 0xC0) == 0x80))

#define utf8_is_codep3(s)                                                      \
    (((((s)[0] & 0xF0) == 0xE0) && (((s)[1] & 0xC0) == 0x80) &&                \
      (((s)[2] & 0xC0) == 0x80)))

#define utf8_is_codep4(s)                                                      \
    (((((s)[0] & 0xF8) == 0xF0) && (((s)[1] & 0xC0) == 0x80) &&                \
      (((s)[2] & 0xC0) == 0x80) && (((s)[3] & 0xC0) == 0x80)))

/*
    Returns the number of byte a utf8 char takes up
    This function always read 4 bytes, so the char must be padded for such a case
    This function returns 0 if there is invalid utf8
*/
static uint32_t utf8_byte_count(const char *s) {
    return
        1 * utf8_is_codep1(s) +
        2 * utf8_is_codep2(s) +
        3 * utf8_is_codep3(s) +
        4 * utf8_is_codep4(s);
}


/*
    Returns the number of characters the string contains accounting for utf8 chars
*/
static uint64_t utf8_strlen(const char *s) {

    const uint64_t so = strlen(s);

    uint64_t l = 0;
    uint64_t b = 0;

    while (b + 4 < so) {

        uint32_t c = utf8_byte_count(s);
        l++;

        if (c == 0) {
            c = 1;
        }

        b += c;
        s += c;
    }

    while (b < so) {

        if (utf8_is_codep1(s)) {
            b++;
            s++;
        } else if ((so - b) >= 2 && utf8_is_codep2(s)) {
            b += 2;
            s += 2;
        } else if ((so - b) >= 3 && utf8_is_codep3(s)) {
            b += 3;
            s += 3;
        } else if ((so - b) >= 4 && utf8_is_codep4(s)) {
            b += 4;
            s += 4;
        } else {
            b++;
            s++;
        }

        l++;
    }

    return l;
}

#endif