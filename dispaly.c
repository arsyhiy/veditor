#include "dispaly.h"

int print() {
  struct frame f;

  f.buffer = malloc(sizeof(struct  if (!f.buffer)
    return 1;

  strcpy(f.buffer->text, "Hello dynamic");

  while (1) {
    printf("\033[2J\033[H");
    printf("%s\n", f.buffer->text);
  };

  free(f.buffer);
  return 0;
};
