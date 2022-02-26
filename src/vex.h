#ifndef _VEX_H
#define _VEX_H

#include <stdint.h>
#include <stddef.h>

struct buffer {
        uint8_t *data;
        size_t len;
};

void vex_init(struct buffer*);

#endif
