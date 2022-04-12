#include "common.h"

#include "vex.h"

#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>

uint64_t apply_byte_ordering(uint64_t addr)
{
        return ROUND_DOWN(addr, state.word_size) +
                (state.word_size - addr % state.word_size - 1);
}

bool is_hexit(char c)
{
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

char byte_to_ascii(uint8_t byte)
{
        if (byte < '!' || byte > '~') {
                return '.';
        }

        return (char)byte;
}

uint8_t hexit_to_int(char c)
{
        c = tolower(c);
        if (c <= '9') {
                return c - '0';
        }

        return c - 'a' + 10;
}

uint8_t read_nibble(void)
{
        char c1 = '\0';
        while (!is_hexit(c1)) {
                c1 = tolower(getch());
        }

        return hexit_to_int(c1);
}

uint64_t prompt_number(void)
{
        char buf[67] = { '\0' };

        uint64_t result;
        do {
                errno = 0;
                size_t i = 0;
                int c;
                while (i < 66) {
                        c = getch();
                        if (c == KEY_BACKSPACE || c == KEY_DC) {
                                if (i > 0) {
                                        buf[--i] = '\0';
                                }
                        } else if (c == KEY_ENTER || c == 0x0a) {
                                break;
                        } else {
                                buf[i++] = c;
                        }

                        vex_update_custom_status(buf);
                }

                switch (buf[1]) {
                        case 'x':
                                result = strtol(buf + 2, NULL, 16);
                                break;
                        case 'o':
                                result = strtol(buf + 2, NULL, 8);
                                break;
                        case 'b':
                                result = strtol(buf + 2, NULL, 2);
                                break;
                        default:
                                result = strtol(buf, NULL, 10);
                                break;
                }

        } while (errno != 0);

        vex_update_custom_status(NULL);

        return result;
}
