#include "vex.h"

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "mode_normal.h"

struct vex_state state;

int ty, tx;
uint64_t screen_bytes;
const char *file_path;

static const char *status_override = NULL;

void vex_init(struct buffer*, const char*);
static void vex_exit(void);
static void vex_redraw_status(void);
static void vex_redraw_all(void);
static void vex_loop(void);

void vex_init(struct buffer *data, const char *file) {
        state.data          = data;
        state.current_mode  = &mode_normal;
        state.offset_cursor = 0;
        state.offset_screen = 0;
        state.word_size     = 1;
        memset(state.mark_cursoroff, 0, sizeof(state.mark_cursoroff));
        memset(state.mark_screenoff, 0, sizeof(state.mark_screenoff));
        state.running       = true;

        file_path = file;

        initscr();
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);

        noecho();
        keypad(stdscr, true);

        getmaxyx(stdscr, ty, tx);
        screen_bytes = 0x10 * (ty - 1);

        vex_redraw_all();
        refresh();

        vex_loop();
}

void vex_exit(void)
{
        // TODO replace with save/quit prompt

        endwin();
        free(state.data->data);
}

void vex_redraw_status(void)
{
        // clear
        int py, px;
        getyx(stdscr, py, px);

        move(0, 0);
        clrtoeol();
        move(py, px);

        if (status_override == NULL) {
                mvprintw(0, 0, "one word = %-2dbytes", state.word_size);
        } else {
                mvprintw(0, 0, "%s", status_override);
        }

        refresh();
}

void vex_redraw_all(void)
{
        clear();

        // status
        vex_redraw_status();

        // header
        for (int bytei = 0; bytei < 16; bytei += state.word_size) {
                mvprintw(0,
                         22 + 2 * (bytei + state.word_size) + bytei / state.word_size,
                         "%02x ", bytei);
        }

        // data
        for (int y = 0; y < ty - 1; y++) {
                uint64_t row_addr = state.offset_screen + 0x10 * y;
                mvprintw(1 + y, 0, "0x%016lx", row_addr);
                int cx = 24;
                for (int bytei = 0; bytei < 16; bytei++) {
                        uint64_t addr = apply_byte_ordering(row_addr + bytei);
                        uint8_t value = vex_data_read(addr);
                        /* numeric values */
                        if (bytei & 1) {
                                attron(COLOR_PAIR(1));
                        }
                        mvprintw(1 + y, cx, "%02x", value);
                        if (bytei & 1) {
                                attroff(COLOR_PAIR(1));
                        }

                        cx += 2;
                        if ((bytei + 1) % state.word_size == 0) {
                                cx++;
                        }

                        /* ascii values */
                        uint64_t ca = apply_byte_ordering(state.offset_cursor);
                        if (ca == addr) {
                                attron(COLOR_PAIR(2));
                        }
                        mvaddch(1 + y,
                                24 + 33 + (16 / state.word_size - 1) + bytei,
                                byte_to_ascii(value));
                        if (ca == addr) {
                                attroff(COLOR_PAIR(2));
                        }
                }
        }

        // set cursor position
        uint64_t relative_screen_addr = state.offset_cursor - state.offset_screen;
        int cy = 1 + relative_screen_addr / 0x10;
        uint8_t bytei = relative_screen_addr & 0xf;
        int cx = 24 + 2 * bytei + bytei / state.word_size;
        move(cy, cx);

        refresh();
}

void vex_loop(void)
{
        while (state.running) {
                int c = getch();

                state.current_mode->process_input(c);

                vex_redraw_all();
        }

        vex_exit();
}

void vex_change_offset(int64_t delta)
{
        state.offset_cursor += delta;
        if (state.offset_cursor < state.offset_screen) {
                state.offset_screen = ROUND_DOWN(state.offset_cursor, 0x10);
        } else if (state.offset_cursor >= state.offset_screen + screen_bytes) {
                uint64_t top = ROUND_DOWN(state.offset_cursor, 0x10);
                state.offset_screen = top - screen_bytes + 0x10;
        }
}

void vex_set_offset(uint64_t offset)
{
        vex_change_offset(offset - state.offset_cursor);
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
                vex_redraw_all();
        }
}

void vex_update_custom_status(const char *status)
{
        status_override = status;
        vex_redraw_status();
}
