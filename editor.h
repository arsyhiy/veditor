
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *data;

  size_t size;
  size_t gap_start;
  size_t gap_end;

  size_t len;
} Buffer;

typedef struct {
  size_t row;
  size_t col;
  size_t offset;
} Cursor;

typedef struct {
  Buffer *buffer;
  Cursor cursor;

  int numrows;

  int cx, cy;
  int rowoff;
  int coloff;

  int screenrows;
  int screencols;

  int insert_mode;

  char filename[256];

  struct Row {
    char *data;
    int len;
    int cap;
  } *rows;

} Editor;

void editor_open(Editor *E, const char *filename);
void editor_process_key(Editor *E);
void editor_refresh_screen(Editor *E);
void init(Editor *E);
