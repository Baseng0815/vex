#include "vex.h"

#include <stdlib.h>
#include <ncurses.h>

#include "mode_normal.h"

struct vex_state state;

int ty, tx;
uint64_t screen_bytes;
bool running;

void vex_init(struct buffer*);
static void vex_exit(void);
static void vex_draw(void);
static void vex_loop(void);

void vex_init(struct buffer *data) {
        state.data          = data;
        state.current_mode  = &mode_normal;
        state.offset_data   = 0;
        state.offset_screen = 0;
        state.word_size     = 1;
        state.running       = true;

        initscr();
        noecho();
        keypad(stdscr, true);

        getmaxyx(stdscr, ty, tx);
        screen_bytes = 0x10 * (ty - 1);

        running = true;
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

        sprintf(buf, "one word = %d bytes", state.word_size);
        mvprintw(0, 0, buf);

        for (int bytei = 0; bytei < 16; bytei += state.word_size) {
                sprintf(buf, "%02x ", bytei);
                mvprintw(0, 24 + 2 * bytei + bytei / state.word_size, buf);
        }

        for (int y = 0; y < ty - 1; y++) {
                uint64_t row_addr = state.offset_screen + 0x10 * y;
                sprintf(buf, "0x%016lx", row_addr);
                mvprintw(1 + y, 0, buf);
                int cx = 24;
                for (int bytei = 0; bytei < 16; bytei++) {
                        uint64_t addr = row_addr + bytei;
                        sprintf(buf, "%02x", vex_data_read(addr));
                        mvprintw(1 + y, cx, buf);
                        cx += 2;
                        if ((bytei + 1) % state.word_size == 0) {
                                cx++;
                        }
                }
        }

        uint64_t relative_screen_addr = state.offset_data - state.offset_screen;
        int cy = 1 + relative_screen_addr / 0x10;
        uint8_t bytei = relative_screen_addr & 0xf;
        int cx = 24 + 2 * bytei + bytei / state.word_size;
        move(cy, cx);
}

void vex_loop(void)
{
        while (state.running) {
                char c = getch();

                state.current_mode->process_char(c);

                clear();
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

void vex_set_offset(uint64_t offset)
{
        vex_change_offset(offset - state.offset_data);
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
                vex_draw();
        }
}
