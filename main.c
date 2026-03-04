#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>

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

Buffer *init_buffer(size_t initial_size) {
  Buffer *buf = malloc(sizeof(Buffer));
  if (!buf)
    return NULL;

  buf->data = malloc(initial_size);
  buf->size = initial_size;
  buf->len = 0;

  buf->gap_start = 0;
  buf->gap_end = initial_size;

  return buf;
}

Editor *init_editor(void) {
  Editor *ed = malloc(sizeof(Editor));
  if (!ed)
    return NULL;

  /* Buffer */
  ed->buffer = init_buffer(1024);

  /* Cursor */
  ed->cursor.row = 0;
  ed->cursor.col = 0;
  ed->cursor.offset = 0;

  return ed;
}

void init(Editor *E) {

  setlocale(LC_ALL, "");

  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);

  init_editor();
}

static void draw_row(Editor *E, int filerow) {
  int y = filerow - E->rowoff;

  if (y < 0 || y >= E->screenrows)
    return;

  move(y, 0);
  clrtoeol();

  if (filerow >= E->numrows) {
    mvprintw(y, 0, "~");
    return;
  }

  struct Row *row = &E->rows[filerow];

  mvprintw(y, 0, "%.*s", E->screencols, row->data + E->coloff);
}
void editor_refresh_screen(Editor *E) {
  //  editor_scroll(E);
  //  clamp_cursor(E);

  for (int i = 0; i < E->screenrows; i++)
    draw_row(E, i + E->rowoff);

  attron(A_REVERSE);

  move(E->screenrows, 0);

  char mode[32];
  snprintf(mode, sizeof(mode), "CTRL+S save | CTRL+Q quit | %s",
           E->insert_mode ? "INSERT" : "NORMAL");

  printw("%-*s", E->screencols, mode);

  attroff(A_REVERSE);

  move(E->cy - E->rowoff, E->cx - E->coloff);

  refresh();
}

int main() {
    Editor E;

  initscr();
  if (initscr() == NULL) {
    fprintf(stderr, "Не удалось инициализировать ncurses\n");
    return 1;
  }

  init(&E);

  while (1) {
    editor_refresh_screen(&E);
    
  }

  endwin();
  return 0;
}
