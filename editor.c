#include "editor.h"

static void clamp_cursor(Editor *E) {
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

static void editor_save(Editor *E) {
  if (!E->filename[0])
    return;

  FILE *fp = fopen(E->filename, "w");
  if (!fp)
    return;

  for (int i = 0; i < E->numrows; i++)
    fprintf(fp, "%s\n", E->rows[i].data);

  fclose(fp);
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

static void editor_insert_char(Editor *E, int c) {
  Buffer *row = &E->rows[E->cursor.row];

  if (E->cursor.col > row->len)
    E->cursor.col = row->len;

  if (row->len + 2 > row->size) {

    int newcap = row->size * 2 + 16;

    char *newdata = realloc(row->data, newcap);
    if (!newdata)
      abort();

    row->data = newdata;
    row->size = newcap;
  }

  memmove(row->data + E->cursor.col + 1, row->data + E->cursor.col,
          row->len - E->cursor.col + 1);

  row->data[E->cursor.col] = (char)c;
  row->len++;

  E->cursor.col++;
}

static void editor_insert_newline(Editor *E) {
  E->rows = realloc(E->rows, sizeof(*E->rows) * (E->numrows + 1));

  memmove(&E->rows[E->cursor.row + 1], &E->rows[E->cursor.row],
          sizeof(*E->rows) * (E->numrows - E->cursor.row));

  Buffer *cur = &E->rows[E->cursor.row];

  int right_len = cur->len - E->cursor.col;

  Buffer newrow;
  newrow.size = right_len + 32;
  newrow.len = right_len;
  newrow.data = calloc(1, newrow.size);

  if (right_len > 0)
    memcpy(newrow.data, cur->data + E->cursor.col, right_len);

  // c ur->data[E->cx] = 0;
  //  cur->len = E->cx;

  // этот хак в полне рабочий но по какой то причине конец файла имеет несколько
  // невидимых символов которых если удалять удаляет видимые символы
  if (cur->size <= E->cursor.col) {
    cur->size = E->cursor.col + 1;
    cur->data = realloc(cur->data, cur->size);
  }

  cur->data[E->cursor.col] = '\0';
  cur->len = E->cursor.col;

  E->rows[E->cursor.row + 1] = newrow;

  E->numrows++;

  E->cursor.row++;
  E->cursor.col = 0;
}

static void editor_backspace(Editor *E) {
  if (E->cursor.row >= E->numrows)
    return;

  Buffer *row = &E->rows[E->cursor.row];

  if (E->cursor.col > 0) {
    memmove(row->data + E->cursor.col - 1, row->data + E->cursor.col,
            row->len - E->cursor.col + 1);

    row->len--;
    E->cursor.col--;
    return;
  }

  if (E->cursor.row == 0)
    return;

  Buffer *prev = &E->rows[E->cursor.row - 1];
  Buffer *cur = row;

  prev->data = realloc(prev->data, prev->len + cur->len + 32);

  memcpy(prev->data + prev->len, cur->data, cur->len + 1);

  prev->len += cur->len;

  free(cur->data);

  memmove(&E->rows[E->cursor.row], &E->rows[E->cursor.row + 1],
          sizeof(*E->rows) * (E->numrows - E->cursor.row - 1));

  E->numrows--;

  E->cursor.row--;
  E->cursor.col = prev->len;
}

void editor_process_key(Editor *E)
{
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

        else if (c == KEY_UP) {
            if (E->cursor.row > 0)
                E->cursor.row--;
        }

        else if (c == KEY_DOWN) {
            if (E->cursor.row + 1 < E->numrows)
                E->cursor.row++;
        }

        else if (c == KEY_LEFT) {

            if (E->cursor.col > 0) {
                E->cursor.col--;
            }
            else if (E->cursor.row > 0) {
                /* переход на предыдущую строку */
                E->cursor.row--;
                E->cursor.col = E->rows[E->cursor.row].len;
            }
        }

        else if (c == KEY_RIGHT) {

            if (E->cursor.row < E->numrows) {

                int len = E->rows[E->cursor.row].len;

                if (E->cursor.col < len) {
                    E->cursor.col++;
                }
                else if (E->cursor.row + 1 < E->numrows) {
                    /* переход на следующую строку */
                    E->cursor.row++;
                    E->cursor.col = 0;
                }
            }
        }

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

void editor_open(Editor *E, const char *filename) {
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

    Buffer *row = &E->rows[E->numrows];

    row->size = l + 32;
    row->len = l;
    row->data = malloc(row->size);

    if (!row->data)
      abort();

    memcpy(row->data, line, l + 1);

    E->numrows++;
  }

  free(line);
  fclose(fp);
}

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

  E->insert_mode = 0;

  E->rows = malloc(sizeof(Buffer));
  if (!E->rows)
    abort();

  E->numrows = 1;

  buffer_init(&E->rows[0], 32);

  E->filename[0] = 0;
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
           E->insert_mode ? "INSERT" : "NORMAL");

  printw("%-*s", E->screencols, mode);

  attroff(A_REVERSE);

  move(E->cursor.row - E->rowoff, E->cursor.col - E->coloff);

  refresh();
}
