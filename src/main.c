#include "vex.h"

#include <stdio.h>
#include <stdlib.h>

void file_read(struct buffer *buffer, const char *file)
{
        FILE *filp = fopen(file, "r");
        if (filp == NULL) {
                fprintf(stderr, "couldn't read file %s\n", file);
                exit(1);
        }

        fseek(filp, 0, SEEK_END);
        buffer->len = ftell(filp);
        rewind(filp);

        buffer->data = (uint8_t*)malloc(buffer->len);
        fread(buffer->data, buffer->len, 1, filp);
        fclose(filp);
}

int main(int argc, char *argv[])
{
        if (argc < 2) {
                fprintf(stderr, "no input file\n");
                return 1;
        }

        struct buffer data;
        file_read(&data, argv[1]);
        vex_init(&data, argv[1]);

        return 0;
}
