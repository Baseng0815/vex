#include "mode_insert.h"

#include "vex.h"

static void process_char(char c);

struct mode mode_insert = {
        .process_char = process_char
};

void process_char(char c)
{
        c++;
}
