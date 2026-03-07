#pragma once

#include <limits.h>
#include <locale.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

#include "undo.h"

typedef struct {
  size_t row;
  size_t col;
  size_t offset;
} Cursor;

typedef enum {
    STATE_NORMAL,
    STATE_INSERT,
    STATE_VISUAL
} EditorState;

typedef struct {
  Cursor cursor;
  Buffer *rows;

  int rowoff;
  int coloff;

  size_t numrows;

  size_t screenrows;
  size_t screencols;

  EditorState editor_state;

  /* undo system */
  UndoOp *undo_stack;
  size_t undo_len;
  size_t undo_cap;

  char filename[PATH_MAX];

} Editor;

void init(Editor *E);
