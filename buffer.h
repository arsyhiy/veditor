#include <locale.h>
#include <stddef.h>
#include <stddef.h> // size_t
#include <stdlib.h> // malloc, calloc, realloc, free
#include <string.h> // memcpy, memmove

typedef struct {
  char *data;

  size_t size; // total capacity
  size_t gap_start;
  size_t gap_end;

  size_t len; // text length
} Buffer;

void buffer_init(Buffer *b, size_t cap);
void buffer_free(Buffer *b);

void buffer_insert(Buffer *b, size_t pos, char c);
void buffer_delete(Buffer *b, size_t pos);
