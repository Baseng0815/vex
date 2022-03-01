#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ROUND_DOWN(V,T) ((V) & ~((T) - 1))

struct buffer {
        uint8_t *data;
        size_t len;
};

struct mode {
        void (*process_char)(char);
};

struct vex_state {
        struct buffer *data;
        struct mode *current_mode;
        uint64_t offset_data;
        uint64_t offset_screen;
        uint8_t word_size; // in bytes (1,2,4,8)
        uint64_t marks[256];
        bool running;
};

bool is_hexit(char);
uint8_t hexit_to_int(char);

/* read one hex digit from user input and return the corresponding nibble
 * blocks until a valid hex digit is read */
uint8_t read_nibble(void);

#endif
