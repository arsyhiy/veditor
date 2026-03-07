#include "buffer.h"

void buffer_init(Buffer *b, size_t cap) {
  b->data = calloc(1, cap);
  if (!b->data)
    abort();

  b->size = cap;
  b->gap_start = 0;
  b->gap_end = cap;
  b->len = 0;
}

static void buffer_grow(Buffer *b, size_t need) {
  if (b->size >= need)
    return;

  size_t newcap = b->size * 2 + 32;
  if (newcap < need)
    newcap = need;

  char *newdata = calloc(1, newcap);
  if (!newdata)
    abort();

  size_t left = b->gap_start;
  size_t right = b->size - b->gap_end;

  memcpy(newdata, b->data, left);
  memcpy(newdata + newcap - right, b->data + b->gap_end, right);

  free(b->data);

  b->data = newdata;
  b->gap_end = newcap - right;
  b->size = newcap;
}

static void buffer_move_gap(Buffer *b, size_t pos) {
  if (pos > b->len)
    pos = b->len;

  if (pos < b->gap_start) {

    size_t shift = b->gap_start - pos;

    memmove(b->data + b->gap_end - shift, b->data + pos, shift);

    b->gap_end -= shift;
    b->gap_start = pos;

  } else if (pos > b->gap_start) {

    size_t shift = pos - b->gap_start;

    memmove(b->data + b->gap_start, b->data + b->gap_end, shift);

    b->gap_start += shift;
    b->gap_end += shift;
  }
}

void buffer_insert(Buffer *b, size_t pos, char c) {
  if (pos > b->len)
    pos = b->len;

  buffer_grow(b, b->len + 2);
  buffer_move_gap(b, pos);

  b->data[b->gap_start] = c;
  b->gap_start++;

  b->len++;
}

void buffer_delete(Buffer *b, size_t pos) {
  if (pos >= b->len)
    return;

  buffer_move_gap(b, pos);

  if (b->gap_start < b->gap_end)
    b->gap_end++;

  b->len--;
}

char buffer_get(Buffer *b, size_t pos) {
  if (pos >= b->len)
    return 0;

  size_t gap_size = b->gap_end - b->gap_start;

  if (pos < b->gap_start)
    return b->data[pos];

  return b->data[pos + gap_size];
}
