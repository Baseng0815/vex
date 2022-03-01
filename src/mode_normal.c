#include "mode_normal.h"

#include "vex.h"

#include <ncurses.h>

static void replace(void);

static void process_char(char c);

struct mode mode_normal = {
        .process_char = process_char
};

void process_char(char c)
{
        switch (c) {
                case 'q':
                        state.running = false;
                        break;
                case 'h':
                        vex_change_offset(-1);
                        break;
                case 'j':
                        vex_change_offset(0x10);
                        break;
                case 'k':
                        vex_change_offset(-0x10);
                        break;
                case 'l':
                        vex_change_offset(1);
                        break;
                case 'w':
                        vex_set_offset(ROUND_DOWN(state.offset_data +
                                                  state.word_size,
                                                  state.word_size));
                        break;
                case 'b':
                        vex_set_offset(ROUND_DOWN(state.offset_data -
                                                  state.word_size,
                                                  state.word_size));
                        break;
                case 'e':
                        vex_set_offset(ROUND_DOWN(state.offset_data + 1,
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
                        vex_set_offset(ROUND_DOWN(state.offset_data, 0x10));
                        break;
                case '$':
                        vex_set_offset(ROUND_DOWN(state.offset_data + 0x10,
                                                  0x10) - 1);
                        break;
                case '+':
                        if (state.word_size < 8) {
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
                        state.marks[(size_t)c] = state.offset_data;
                        break;
                }
                case '\'': {
                        char c = getch();
                        uint64_t addr = state.marks[(size_t)c];
                        if (addr) {
                                vex_set_offset(addr);
                        }
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
        uint8_t value = vex_data_read(state.offset_data);

        // TODO take word size into account (replace whole words)
        uint8_t v = read_nibble();
        value &= 0x0f;
        value |= (v << 4);
        vex_data_write(state.offset_data, value);

        v = read_nibble();
        value &= 0xf0;
        value |= (v << 0);
        vex_data_write(state.offset_data, value);
}