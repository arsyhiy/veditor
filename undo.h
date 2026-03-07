#pragma once

#include <stdlib.h>

typedef enum {
    UNDO_INSERT,
    UNDO_DELETE,
    UNDO_NEWLINE,
    UNDO_BACKSPACE
} UndoType;


typedef struct {
    UndoType type;

    size_t row;
    size_t col;

    char ch;
} UndoOp;
