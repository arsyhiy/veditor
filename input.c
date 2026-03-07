/* #include "editor.h" */

/* static void editor_save(Editor *E) { */
/*   if (!E->filename[0]) */
/*     return; */

/*   FILE *fp = fopen(E->filename, "w"); */
/*   if (!fp) */
/*     return; */

/*   for (int i = 0; i < E->numrows; i++) */
/*     fprintf(fp, "%s\n", E->rows[i].data); */

/*   fclose(fp); */
/* } */


/* static void editor_insert_char(Editor *E, int c) { */
/*   Buffer *row = &E->rows[E->cursor.row]; */

/*   if (E->cursor.col > row->len) */
/*     E->cursor.col = row->len; */

/*   if (row->len + 2 > row->size) { */

/*     int newcap = row->size * 2 + 16; */

/*     char *newdata = realloc(row->data, newcap); */
/*     if (!newdata) */
/*       abort(); */

/*     row->data = newdata; */
/*     row->size = newcap; */
/*   } */

/*   memmove(row->data + E->cursor.col + 1, row->data + E->cursor.col, */
/*           row->len - E->cursor.col + 1); */

/*   row->data[E->cursor.col] = (char)c; */
/*   row->len++; */

/*   E->cursor.col++; */
/* } */


/* static void undo_push(Editor *E, UndoOp op) */
/* { */
/*     if (E->undo_len + 1 > E->undo_cap) { */

/*         size_t newcap = E->undo_cap * 2 + 32; */

/*         UndoOp *newbuf = */
/*             realloc(E->undo_stack, newcap * sizeof(UndoOp)); */

/*         if (!newbuf) */
/*             abort(); */

/*         E->undo_stack = newbuf; */
/*         E->undo_cap = newcap; */
/*     } */

/*     E->undo_stack[E->undo_len++] = op; */
/* } */


/* static void editor_undo(Editor *E) */
/* { */
/*     if (E->undo_len == 0) */
/*         return; */

/*     UndoOp op = E->undo_stack[--E->undo_len]; */

/*     E->cursor.row = op.row; */
/*     E->cursor.col = op.col; */

/*     Buffer *row = &E->rows[op.row]; */

/*     switch (op.type) { */

/*     case UNDO_INSERT: */
/*         buffer_delete(row, op.col); */
/*         break; */

/*     case UNDO_DELETE: */
/*         buffer_insert(row, op.col, op.ch); */
/*         break; */

/*     default: */
/*         break; */
/*     } */
/* } */

/* static void editor_insert_newline(Editor *E) { */
/*   E->rows = realloc(E->rows, sizeof(*E->rows) * (E->numrows + 1)); */

/*   memmove(&E->rows[E->cursor.row + 1], &E->rows[E->cursor.row], */
/*           sizeof(*E->rows) * (E->numrows - E->cursor.row)); */

/*   Buffer *cur = &E->rows[E->cursor.row]; */

/*   int right_len = cur->len - E->cursor.col; */

/*   Buffer newrow; */
/*   newrow.size = right_len + 32; */
/*   newrow.len = right_len; */
/*   newrow.data = calloc(1, newrow.size); */

/*   if (right_len > 0) */
/*     memcpy(newrow.data, cur->data + E->cursor.col, right_len); */

/*   // c ur->data[E->cx] = 0; */
/*   //  cur->len = E->cx; */

/*   // этот хак в полне рабочий но по какой то причине конец файла имеет несколько */
/*   // невидимых символов которых если удалять удаляет видимые символы */
/*   if (cur->size <= E->cursor.col) { */
/*     cur->size = E->cursor.col + 1; */
/*     cur->data = realloc(cur->data, cur->size); */
/*   } */

/*   cur->data[E->cursor.col] = '\0'; */
/*   cur->len = E->cursor.col; */

/*   E->rows[E->cursor.row + 1] = newrow; */

/*   E->numrows++; */

/*   E->cursor.row++; */
/*   E->cursor.col = 0; */
/* } */

/* static void editor_backspace(Editor *E) { */
/*   if (E->cursor.row >= E->numrows) */
/*     return; */

/*   Buffer *row = &E->rows[E->cursor.row]; */

/*   if (E->cursor.col > 0) { */
/*     memmove(row->data + E->cursor.col - 1, row->data + E->cursor.col, */
/*             row->len - E->cursor.col + 1); */

/*     row->len--; */
/*     E->cursor.col--; */
/*     return; */
/*   } */

/*   if (E->cursor.row == 0) */
/*     return; */

/*   Buffer *prev = &E->rows[E->cursor.row - 1]; */
/*   Buffer *cur = row; */

/*   prev->data = realloc(prev->data, prev->len + cur->len + 32); */

/*   memcpy(prev->data + prev->len, cur->data, cur->len + 1); */

/*   prev->len += cur->len; */

/*   free(cur->data); */

/*   memmove(&E->rows[E->cursor.row], &E->rows[E->cursor.row + 1], */
/*           sizeof(*E->rows) * (E->numrows - E->cursor.row - 1)); */

/*   E->numrows--; */

/*   E->cursor.row--; */
/*   E->cursor.col = prev->len; */
/* } */

/* void editor_process_key(Editor *E) */
/* { */
/*     int c = getch(); */

/*     if (E->editor_state != STATE_INSERT) */

/*         if (c == 'i') */
/*             E->editor_state = STATE_INSERT; */

/*         else if (c == 17) { */
/*             endwin(); */
/*             exit(0); */
/*         } */

/*         else if (c == 19) */
/*             editor_save(E); */

/*         else if (c == 26) */
/*           editor_undo(E); */

/*         else if (c == KEY_UP) { */
/*             if (E->cursor.row > 0) */
/*                 E->cursor.row--; */
/*         } */

/*         else if (c == KEY_DOWN) { */
/*             if (E->cursor.row + 1 < E->numrows) */
/*                 E->cursor.row++; */
/*         } */

/*         else if (c == KEY_LEFT) { */

/*             if (E->cursor.col > 0) { */
/*                 E->cursor.col--; */
/*             } */
/*             else if (E->cursor.row > 0) { */
/*                 /\* переход на предыдущую строку *\/ */
/*                 E->cursor.row--; */
/*                 E->cursor.col = E->rows[E->cursor.row].len; */
/*             } */
/*         } */

/*         else if (c == KEY_RIGHT) { */

/*             if (E->cursor.row < E->numrows) { */

/*                 int len = E->rows[E->cursor.row].len; */

/*                 if (E->cursor.col < len) { */
/*                     E->cursor.col++; */
/*                 } */
/*                 else if (E->cursor.row + 1 < E->numrows) { */
/*                     /\* переход на следующую строку *\/ */
/*                     E->cursor.row++; */
/*                     E->cursor.col = 0; */
/*                 } */
/*             } */
/*         } */

/*     } else { */

/*         if (c == 27) */
/*             E->editor_state = STATE_NORMAL; */

/*         else if (c == KEY_BACKSPACE || c == 127) */
/*             editor_backspace(E); */

/*         else if (c == '\n' || c == 10) */
/*             editor_insert_newline(E); */

/*         else if (c >= 32 && c <= 126) */
/*             editor_insert_char(E, c); */
/*     } */
/* } */



#include "editor.h"
#include "undo.h"
#include "display.h"


void undo_push(Editor *E, UndoOp op)
{
    if (E->undo_len + 1 > E->undo_cap) {

        size_t newcap = E->undo_cap * 2 + 32;

        UndoOp *newbuf =
            realloc(E->undo_stack,
                    newcap * sizeof(UndoOp));

        if (!newbuf)
            abort();

        E->undo_stack = newbuf;
        E->undo_cap = newcap;
    }

    E->undo_stack[E->undo_len++] = op;
}


static void editor_save(Editor *E)
{
    if (!E->filename[0])
        return;

    FILE *fp = fopen(E->filename, "w");
    if (!fp)
        return;

    for (size_t i = 0; i < E->numrows; i++)
        fprintf(fp, "%s\n", E->rows[i].data);

    fclose(fp);
}

static void editor_insert_char(Editor *E, int c)
{
    Buffer *row = &E->rows[E->cursor.row];

    if (E->cursor.col > row->len)
        E->cursor.col = row->len;

    undo_push(E, (UndoOp){
        .type = UNDO_INSERT,
        .row = E->cursor.row,
        .col = E->cursor.col,
        .ch = c
    });

    if (row->len + 2 > row->size) {

        size_t newcap = row->size * 2 + 16;

        char *newdata = realloc(row->data, newcap);
        if (!newdata)
            abort();

        row->data = newdata;
        row->size = newcap;
    }

    memmove(row->data + E->cursor.col + 1,
            row->data + E->cursor.col,
            row->len - E->cursor.col + 1);

    row->data[E->cursor.col] = (char)c;
    row->len++;

    E->cursor.col++;
}

static void editor_undo(Editor *E)
{
    if (E->undo_len == 0)
        return;

    UndoOp op = E->undo_stack[--E->undo_len];

    if (op.row >= E->numrows)
        return;

    E->cursor.row = op.row;
    E->cursor.col = op.col;

    Buffer *row = &E->rows[op.row];

    switch (op.type) {

    case UNDO_INSERT:
        buffer_delete(row, op.col);
        break;

    case UNDO_DELETE:
        buffer_insert(row, op.col, op.ch);
        break;

    default:
        break;
    }
}

static void editor_insert_newline(Editor *E)
{
    Buffer *newrows =
        realloc(E->rows, sizeof(Buffer) * (E->numrows + 1));

    if (!newrows)
        abort();

    E->rows = newrows;

    memmove(&E->rows[E->cursor.row + 1],
            &E->rows[E->cursor.row],
            sizeof(Buffer) * (E->numrows - E->cursor.row));

    Buffer *cur = &E->rows[E->cursor.row];

    size_t right_len = cur->len - E->cursor.col;

    Buffer newrow;
    newrow.size = right_len + 32;
    newrow.len = right_len;
    newrow.data = calloc(1, newrow.size);

    if (!newrow.data)
        abort();

    if (right_len > 0)
        memcpy(newrow.data,
               cur->data + E->cursor.col,
               right_len);

    char *newdata = realloc(cur->data, E->cursor.col + 1);
    if (!newdata)
        abort();

    cur->data = newdata;
    cur->data[E->cursor.col] = '\0';
    cur->len = E->cursor.col;

    E->rows[E->cursor.row + 1] = newrow;

    E->numrows++;

    E->cursor.row++;
    E->cursor.col = 0;
}

static void editor_backspace(Editor *E)
{
    if (E->cursor.row >= E->numrows)
        return;

    Buffer *row = &E->rows[E->cursor.row];

    if (E->cursor.col > 0) {

        undo_push(E, (UndoOp){
            .type = UNDO_DELETE,
            .row = E->cursor.row,
            .col = E->cursor.col - 1,
            .ch = row->data[E->cursor.col - 1]
        });

        memmove(row->data + E->cursor.col - 1,
                row->data + E->cursor.col,
                row->len - E->cursor.col + 1);

        row->len--;
        E->cursor.col--;
        return;
    }

    if (E->cursor.row == 0)
        return;

    Buffer *prev = &E->rows[E->cursor.row - 1];
    Buffer *cur = row;

    char *newdata =
        realloc(prev->data, prev->len + cur->len + 32);

    if (!newdata)
        abort();

    prev->data = newdata;

    memcpy(prev->data + prev->len,
           cur->data,
           cur->len + 1);

    prev->len += cur->len;

    free(cur->data);

    memmove(&E->rows[E->cursor.row],
            &E->rows[E->cursor.row + 1],
            sizeof(Buffer) * (E->numrows - E->cursor.row - 1));

    E->numrows--;

    E->cursor.row--;
    E->cursor.col = prev->len;
}

void editor_process_key(Editor *E)
{
  int c = getch();

  if (E->editor_state != STATE_INSERT) {

    if (c == 'i')
      E->editor_state = STATE_INSERT;

    else if (c == 17) {
      endwin();
      exit(0);
    }

    else if (c == 19)
      editor_save(E);

    else if (c == 26)
      editor_undo(E);

    else if (c == KEY_UP && E->cursor.row > 0)
      E->cursor.row--;

    else if (c == KEY_DOWN &&
             E->cursor.row + 1 < E->numrows)
      E->cursor.row++;

    else if (c == KEY_LEFT) {

      if (E->cursor.col > 0)
        E->cursor.col--;

      else if (E->cursor.row > 0) {
        E->cursor.row--;
        E->cursor.col =
          E->rows[E->cursor.row].len;
      }
    }

    else if (c == KEY_RIGHT) {

      if (E->cursor.row < E->numrows) {

        size_t len = E->rows[E->cursor.row].len;

        if (E->cursor.col < len)
          E->cursor.col++;

        else if (E->cursor.row + 1 < E->numrows) {
          E->cursor.row++;
          E->cursor.col = 0;
        }
      }
    }

  } else {

    if (c == 27)
      E->editor_state = STATE_NORMAL;

    else if (c == KEY_BACKSPACE || c == 127)
      editor_backspace(E);

    else if (c == '\n' || c == 10)
      editor_insert_newline(E);

    else if (c >= 32 && c <= 126)
      editor_insert_char(E, c);
  }

  clamp_cursor(E);
}
