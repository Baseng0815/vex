#ifndef _VEX_H
#define _VEX_H

#include "common.h"

extern struct vex_state state;
extern int ty, tx;
extern uint64_t screen_bytes;

void vex_change_offset(int64_t);
void vex_set_offset(uint64_t);
uint8_t vex_data_read(uint64_t);
void vex_data_write(uint64_t, uint8_t);

void vex_update_custom_status(const char*);

void vex_init(struct buffer*);

#endif
