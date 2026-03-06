#include <locale.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *data;
  size_t len;
  size_t cap;

} Buffer;
void buffer_init(Buffer *b, size_t cap);
void buffer_free(Buffer *b);

void buffer_insert(Buffer *b, size_t pos, char c);
void buffer_delete(Buffer *b, size_t pos);
