#include <limits.h>
#include <locale.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

typedef struct {
  size_t row;
  size_t col;
  size_t offset;
} Cursor;

typedef struct {
  Cursor cursor;
  Buffer *rows;

  int rowoff;
  int coloff;

  size_t numrows;

  size_t screenrows;
  size_t screencols;

  int insert_mode;

  char filename[PATH_MAX];

} Editor;

void editor_open(Editor *E, const char *filename);
void editor_process_key(Editor *E);
void editor_refresh_screen(Editor *E);
void init(Editor *E);
