#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buffer {
  char text[1024];
};

struct frame {
  struct buffer *buffer;
};

int print();
