#include "common.h"

#include "vex.h"

#include <ncurses.h>
#include <ctype.h>

uint64_t apply_byte_ordering(uint64_t addr)
{
        return ROUND_DOWN(addr, state.word_size) +
                (state.word_size - addr % state.word_size - 1);
}

bool is_hexit(char c)
{
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

char to_ascii(uint8_t byte)
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
