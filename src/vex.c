#include "vex.h"

#include <stdlib.h>
#include <ncurses.h>

struct buffer *data;
uint8_t data_offset = 0;
bool running = true;

void vex_loop(void);

void vex_init(struct buffer *_data) {
        data = _data;

        initscr();
        noecho();
        keypad(stdscr, true);

        vex_loop();
}

void vex_exit(void)
{
        endwin();
        free(data->data);
}

void vex_loop(void)
{
        while (running) {
                char c = getch();
                switch (c) {
                        case 'q':
                                running = false;
                                break;
                        default:
                                break;
                }

                refresh();
        }

        vex_exit();
}
