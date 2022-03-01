#include "vex.h"

#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

#define ROUND_DOWN(V,T) ((V) & ~((T) - 1))

struct vex_state {
        struct buffer *data;
        uint64_t offset_data;
        uint64_t offset_screen;
} state;

int ty, tx;
uint64_t screen_bytes;
bool running = true;

void vex_init(struct buffer*);
void vex_exit(void);
void vex_draw(void);
void vex_loop(void);

void vex_change_offset(int64_t);
uint8_t vex_data_read(uint64_t);
void vex_data_write(uint64_t, uint8_t);

void vex_init(struct buffer *data) {
        state.data          = data;
        state.offset_data   = 0;
        state.offset_screen = 0;

        initscr();
        noecho();
        keypad(stdscr, true);

        getmaxyx(stdscr, ty, tx);
        screen_bytes = 0x10 * (ty - 1);

        vex_loop();
}

void vex_exit(void)
{
        endwin();
        free(state.data->data);
}

void vex_draw(void)
{

        char buf[24];

        for (int byte = 0; byte < 16; byte++) {
                sprintf(buf, "%02x ", byte);
                mvprintw(0, 24 + 3 * byte, buf);
        }

        for (int y = 0; y < ty - 1; y++) {
                uint64_t row_addr = state.offset_screen + 0x10 * y;
                sprintf(buf, "0x%016lx", row_addr);
                mvprintw(1 + y, 0, buf);
                for (int x = 0; x < 16; x++) {
                        uint64_t addr = row_addr + x;
                        sprintf(buf, "%02x", vex_data_read(addr));
                        mvprintw(1 + y, 24 + 3 * x, buf);
                }
        }

        uint64_t relative_screen_addr = state.offset_data - state.offset_screen;
        int cy = 1 + ROUND_DOWN(relative_screen_addr, 0x10) / 0x10;
        int cx = 24 + 3 * (relative_screen_addr % 0x10);
        move(cy, cx);
}

void vex_loop(void)
{
        while (running) {
                char c = getch();
                switch (c) {
                        case 'q':
                                running = false;
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
                        default:
                                break;
                }

                vex_draw();
                refresh();
        }

        vex_exit();
}

void vex_change_offset(int64_t delta)
{
        state.offset_data += delta;
        if (state.offset_data < state.offset_screen) {
                state.offset_screen = ROUND_DOWN(state.offset_data, 0x10);
        } else if (state.offset_data >= state.offset_screen + screen_bytes) {
                uint64_t top = ROUND_DOWN(state.offset_data, 0x10);
                state.offset_screen = top - screen_bytes + 0x10;
        }
}

uint8_t vex_data_read(uint64_t addr)
{
        if (addr < state.data->len) {
                return state.data->data[addr];
        } else {
                return 0;
        }
}

void vex_data_write(uint64_t addr, uint8_t val)
{
        if (addr < state.data->len) {
                state.data->data[addr] = val;
        }
}
