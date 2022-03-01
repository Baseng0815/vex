#ifndef _VEX_H
#define _VEX_H

#include "common.h"

extern struct vex_state state;
extern int ty, tx;
extern uint64_t screen_bytes;

void vex_change_offset(int64_t);
uint8_t vex_data_read(uint64_t);
void vex_data_write(uint64_t, uint8_t);

void vex_init(struct buffer*);

#endif
