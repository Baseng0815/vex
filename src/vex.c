#include "vex.h"

#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

struct vex_state {
        struct buffer *data;
        uint64_t offset;
} state;

int ty, tx;
bool running = true;

void vex_loop(void);

void vex_init(struct buffer *data) {
        state.data          = data;
        state.offset        = 0;

        initscr();
        noecho();
        keypad(stdscr, true);

        getmaxyx(stdscr, ty, tx);

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
                mvprintw(0, 24 + byte * 3, buf);
        }

        for (int y = 0; y < ty; y++) {
                uint64_t addr = 16 * y + (state.offset & ~0xf);
                sprintf(buf, "0x%016lx", addr);
                mvprintw(y + 1, 0, buf);
                for (int x = 0; x < 16; x++) {
                        size_t index = x + y * 0x10;
                        sprintf(buf, "%02x", state.data->data[index]);
                        mvprintw(y + 1, 24 + x * 3, buf);
                }
        }

        int cx = 24 + (state.offset & 0xf) * 3;
        int cy = (state.offset & ~0xf) / 0x10;
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
                                state.offset--;
                                break;
                        case 'j':
                                state.offset += 0x10;
                                break;
                        case 'k':
                                state.offset -= 0x10;
                                break;
                        case 'l':
                                state.offset++;
                                break;
                        default:
                                break;
                }

                vex_draw();
                refresh();
        }

        vex_exit();
}
