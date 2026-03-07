#include "editor.h"





// Editor *init_editor(void) {
//   Editor *ed = malloc(sizeof(Editor));
//   if (!ed)
//     return NULL;
//
//   /* Buffer */
//   ed->buffer = init_buffer(1024);
//
//   /* Cursor */
//   ed->cursor.row = 0;
//   ed->cursor.col = 0;
//   ed->cursor.offset = 0;
//
//   return ed;
// }

void init(Editor *E) {
  setlocale(LC_ALL, "");

  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);

  getmaxyx(stdscr, E->screenrows, E->screencols);
  E->screenrows--;

  /* cursor */
  E->cursor.col = 0;
  E->cursor.row = 0;
  E->cursor.offset = 0;

  E->rowoff = 0;
  E->coloff = 0;

  E->editor_state = STATE_NORMAL;

  E->rows = malloc(sizeof(Buffer));
  if (!E->rows)
    abort();

  E->numrows = 1;

  buffer_init(&E->rows[0], 32);

  E->filename[0] = 0;
}

