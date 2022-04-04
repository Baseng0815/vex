#include "mode_insert.h"

#include "vex.h"

static void process_input(int c);

struct mode mode_insert = {
        .process_input = process_input
};

void process_input(int c)
{
        c++;
}
