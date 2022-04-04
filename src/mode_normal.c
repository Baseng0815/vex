#include "mode_normal.h"

#include "vex.h"

#include <ncurses.h>

static void replace(void);

static void process_char(int c);

struct mode mode_normal = {
        .process_input = process_char
};

void process_char(int c)
{
        switch (c) {
                case 'q':
                        state.running = false;
                        break;
                case KEY_LEFT:
                case 'h':
                        vex_change_offset(-1);
                        break;
                case KEY_DOWN:
                case 'j':
                        vex_change_offset(0x10);
                        break;
                case KEY_UP:
                case 'k':
                        vex_change_offset(-0x10);
                        break;
                case KEY_RIGHT:
                case 'l':
                        vex_change_offset(1);
                        break;
                case 'w':
                        vex_set_offset(ROUND_DOWN(state.offset_cursor +
                                                  state.word_size,
                                                  state.word_size));
                        break;
                case 'b':
                        vex_set_offset(ROUND_DOWN(state.offset_cursor - 1,
                                                  state.word_size));
                        break;
                case 'e':
                        vex_set_offset(ROUND_DOWN(state.offset_cursor + 1,
                                                  state.word_size)
                                       + state.word_size - 1);
                        break;
                case 'g':
                        vex_set_offset(0);
                        break;
                case 'G':
                        vex_set_offset(state.data->len);
                        break;
                case 'U':
                        vex_change_offset(-screen_bytes);
                        break;
                case 'D':
                        vex_change_offset(screen_bytes);
                        break;
                case '0':
                        vex_set_offset(ROUND_DOWN(state.offset_cursor, 0x10));
                        break;
                case '$':
                        vex_set_offset(ROUND_DOWN(state.offset_cursor + 0x10,
                                                  0x10) - 1);
                        break;
                case '+':
                        if (state.word_size < 16) {
                                state.word_size <<= 1;
                        }
                        break;
                case '-':
                        if (state.word_size > 1) {
                                state.word_size >>= 1;
                        }
                        break;
                case 'm': {
                        char c = getch();
                        state.mark_cursoroff[(size_t)c] = state.offset_cursor;
                        state.mark_screenoff[(size_t)c] = state.offset_screen;
                        break;
                }
                case '\'': {
                        char c = getch();
                        state.offset_cursor = state.mark_cursoroff[(size_t)c];
                        state.offset_screen = state.mark_screenoff[(size_t)c];
                        break;
                }
                case 'r':
                        replace();
                        break;
                default:
                        break;
        }
}

void replace(void)
{
        uint64_t addr = apply_byte_ordering(state.offset_cursor);
        uint8_t value = vex_data_read(addr);

        // TODO take word size into account (replace whole words)
        uint8_t v = read_nibble();
        value &= 0x0f;
        value |= (v << 4);
        vex_data_write(addr, value);

        v = read_nibble();
        value &= 0xf0;
        value |= (v << 0);
        vex_data_write(addr, value);
}
