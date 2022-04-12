#include "mode_normal.h"

#include "vex.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

static void replace(void);
static void seek(void);
static void quit(void);

static void process_char(int c);

struct mode mode_normal = {
        .process_input = process_char
};

void process_char(int c)
{
        switch (c) {
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
                case 'K':
                        vex_change_offset(-screen_bytes);
                        break;
                case 'J':
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
                case ':':
                        seek();
                        break;
                case 'q':
                        quit();
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

void seek(void)
{
        uint64_t target = prompt_number();
        vex_set_offset(target);
}

void quit(void)
{
        vex_update_custom_status("(w)rite/(q)uit/(a)bort");
        char c = '\0';
        while (c != 'a') {
                c = getch();
                switch (c) {
                        case 'w': {
                                FILE *filp = fopen(file_path, "w");
                                if (!filp) {
                                        fprintf(stderr, "couldn't open file for saving\n");
                                        exit(1);
                                }
                                fwrite(state.data->data, 1, state.data->len, filp);
                                state.running = false;
                                return;
                        }
                        case 'q':
                                state.running = false;
                                return;
                        default:
                                continue;
                }
        }

        vex_update_custom_status(NULL);
}
