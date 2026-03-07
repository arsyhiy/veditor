#include "editor.h"

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
