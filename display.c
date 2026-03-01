#include "display.h"

int print(){
    struct frame f;

    f.buffer = malloc(sizeof(struct buffer));
    if (!f.buffer) return 1;

    strcpy(f.buffer->text, "Hello dynamic");

    printf("%s\n", f.buffer->text);

    free(f.buffer);
    return 0;
};
