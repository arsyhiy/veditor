#include"editor.h"


void clamp_cursor(Editor *E) {
  if (E->cursor.row < 0)
    E->cursor.row = 0;

  if (E->numrows == 0)
    return;

  if (E->cursor.row >= E->numrows)
    E->cursor.row = E->numrows - 1;

  int len = E->rows[E->cursor.row].len;

  if (E->cursor.col > len)
    E->cursor.col = len;

  if (E->cursor.col < 0)
    E->cursor.col = 0;
}



static void editor_scroll(Editor *E) {
  if (E->cursor.row < E->rowoff)
    E->rowoff = E->cursor.row;

  if (E->cursor.row >= E->rowoff + E->screenrows)
    E->rowoff = E->cursor.row - E->screenrows + 1;

  int len = E->rows[E->cursor.row].len;

  if (E->cursor.col < E->coloff)
    E->coloff = E->cursor.col;

  if (E->cursor.col >= E->coloff + E->screencols)
    E->coloff = E->cursor.col - E->screencols + 1;
}

static void draw_row(Editor *E, int filerow)
{
    int y = filerow - E->rowoff;

    if (y < 0 || y >= E->screenrows)
        return;

    move(y, 0);
    clrtoeol();

    if (filerow >= E->numrows) {
        mvprintw(y, 0, "~");
        return;
    }

    Buffer *row = &E->rows[filerow];

    if (E->coloff >= row->len)
        return;

    int len = row->len - E->coloff;

    if (len > E->screencols)
        len = E->screencols;

    mvprintw(y, 0, "%.*s", len, row->data + E->coloff);
}

void editor_refresh_screen(Editor *E) {
  editor_scroll(E);
  clamp_cursor(E);

  for (int i = 0; i < E->screenrows; i++)
    draw_row(E, i + E->rowoff);

  attron(A_REVERSE);

  move(E->screenrows, 0);

  char mode[32];
  snprintf(mode, sizeof(mode), "CTRL+S save | CTRL+Q quit | %s",
           E->editor_state == STATE_INSERT ? "INSERT" : "NORMAL");

  printw("%-*s", E->screencols, mode);

  attroff(A_REVERSE);

  move(E->cursor.row - E->rowoff, E->cursor.col - E->coloff);

  refresh();
}
