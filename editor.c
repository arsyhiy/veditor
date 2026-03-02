#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"

/* ---------- helpers ---------- */

static void clamp_cursor(EditorState *E) {
  if (E->cy < 0)
    E->cy = 0;

  if (E->cy >= E->numrows)
    E->cy = E->numrows - 1;

  int len = E->rows[E->cy].len;

  if (E->cx > len)
    E->cx = len;

  if (E->cx < 0)
    E->cx = 0;
}

/* ---------- init ---------- */

void editor_init(EditorState *E) {
  setlocale(LC_ALL, "");

  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(1);

  getmaxyx(stdscr, E->screenrows, E->screencols);
  E->screenrows--;

  E->cx = E->cy = 0;
  E->rowoff = E->coloff = 0;

  E->insert_mode = 0;

  /* allocate one empty row */

  E->rows = malloc(sizeof(*E->rows));
  E->numrows = 1;

  E->rows[0].cap = 32;
  E->rows[0].len = 0;
  E->rows[0].data = calloc(1, 32);

  E->filename[0] = 0;
}

/* ---------- open file ---------- */

void editor_open(EditorState *E, const char *filename) {
  strncpy(E->filename, filename, 255);

  FILE *fp = fopen(filename, "r");
  if (!fp)
    return;

  char *line = NULL;
  size_t cap = 0;

  while (getline(&line, &cap, fp) != -1) {
    int l = strlen(line);

    if (l && line[l - 1] == '\n')
      line[l - 1] = 0;

    E->rows = realloc(E->rows, sizeof(*E->rows) * (E->numrows + 1));

    struct Row *row = &E->rows[E->numrows];

    row->cap = l + 32;
    row->len = l;
    row->data = malloc(row->cap);

    memcpy(row->data, line, l + 1);

    E->numrows++;
  }

  free(line);
  fclose(fp);
}

/* ---------- save ---------- */

static void editor_save(EditorState *E) {
  if (!E->filename[0])
    return;

  FILE *fp = fopen(E->filename, "w");
  if (!fp)
    return;

  for (int i = 0; i < E->numrows; i++)
    fprintf(fp, "%s\n", E->rows[i].data);

  fclose(fp);
}

/* ---------- scroll ---------- */

static void editor_scroll(EditorState *E) {
  if (E->cy < E->rowoff)
    E->rowoff = E->cy;

  if (E->cy >= E->rowoff + E->screenrows)
    E->rowoff = E->cy - E->screenrows + 1;

  int len = E->rows[E->cy].len;

  if (E->cx < E->coloff)
    E->coloff = E->cx;

  if (E->cx >= E->coloff + E->screencols)
    E->coloff = E->cx - E->screencols + 1;
}

/* ---------- editing ---------- */

static void editor_insert_char(EditorState *E, int c) {
  struct Row *row = &E->rows[E->cy];

  if (row->len + 2 > row->cap) {
    int newcap = row->cap * 2 + 16;
    row->data = realloc(row->data, newcap);
    row->cap = newcap;
  }

  memmove(row->data + E->cx + 1, row->data + E->cx, row->len - E->cx + 1);

  row->data[E->cx] = c;
  row->len++;

  E->cx++;
}

/* ---------- newline ---------- */

static void editor_insert_newline(EditorState *E) {
  E->rows = realloc(E->rows, sizeof(*E->rows) * (E->numrows + 1));

  memmove(&E->rows[E->cy + 1], &E->rows[E->cy],
          sizeof(*E->rows) * (E->numrows - E->cy));

  struct Row *cur = &E->rows[E->cy];

  int right_len = cur->len - E->cx;

  struct Row newrow;
  newrow.cap = right_len + 32;
  newrow.len = right_len;
  newrow.data = calloc(1, newrow.cap);

  if (right_len > 0)
    memcpy(newrow.data, cur->data + E->cx, right_len);

  cur->data[E->cx] = 0;
  cur->len = E->cx;

  E->rows[E->cy + 1] = newrow;

  E->numrows++;

  E->cy++;
  E->cx = 0;
}

/* ---------- backspace ---------- */

static void editor_backspace(EditorState *E) {
  if (E->cy >= E->numrows)
    return;

  struct Row *row = &E->rows[E->cy];

  if (E->cx > 0) {
    memmove(row->data + E->cx - 1, row->data + E->cx, row->len - E->cx + 1);

    row->len--;
    E->cx--;
    return;
  }

  if (E->cy == 0)
    return;

  struct Row *prev = &E->rows[E->cy - 1];
  struct Row *cur = row;

  prev->data = realloc(prev->data, prev->len + cur->len + 32);

  memcpy(prev->data + prev->len, cur->data, cur->len + 1);

  prev->len += cur->len;

  free(cur->data);

  memmove(&E->rows[E->cy], &E->rows[E->cy + 1],
          sizeof(*E->rows) * (E->numrows - E->cy - 1));

  E->numrows--;

  E->cy--;
  E->cx = prev->len;
}

/* ---------- drawing ---------- */

static void draw_row(EditorState *E, int filerow) {
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

/* ---------- refresh ---------- */

void editor_refresh_screen(EditorState *E) {
  editor_scroll(E);
  clamp_cursor(E);

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

/* ---------- input ---------- */

void editor_process_key(EditorState *E) {
  int c = getch();

  if (!E->insert_mode) {
    if (c == 'i')
      E->insert_mode = 1;

    else if (c == 17) {
      endwin();
      exit(0);
    }

    else if (c == 19)
      editor_save(E);

    else if (c == KEY_UP)
      E->cy--;

    else if (c == KEY_DOWN)
      E->cy++;

    else if (c == KEY_LEFT)
      E->cx--;

    else if (c == KEY_RIGHT)
      E->cx++;
  } else {
    if (c == 27)
      E->insert_mode = 0;

    else if (c == KEY_BACKSPACE || c == 127)
      editor_backspace(E);

    else if (c == '\n' || c == 10)
      editor_insert_newline(E);

    else if (c >= 32 && c <= 126)
      editor_insert_char(E, c);
  }
}
