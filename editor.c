#include "editor.h"

void init(Editor *E) {
  setlocale(LC_ALL, "");

  initscr(); // TODO: переделать инитиализацию интерфейса в отдельную функцию display.c
  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);

  // проще говоря нижняя строка получате значения для размера экрана
  getmaxyx(stdscr, E->screenrows, E->screencols); // TODO : вызывается он один раз надо будет сделать так чтобы был флаг если пользователь изменил размер экрана.
  
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


void shutdown(Editor *E) {
  // TODO: закрыть любые или убить процессы а также очистить занимаемо место в ram.

  // destroy buffer
  // destroy_buffer();

  // destroy undo stack
  // destroy_undo_stack();
  
  
// закрытие интерфейса
  endwin();
  
}
