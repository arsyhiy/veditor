#ifndef EDITOR_H
#define EDITOR_H

typedef struct {

  struct Row {
    char *data;
    int len;
    int cap;
  } *rows;

  int numrows;

  int cx, cy;
  int rowoff;
  int coloff;

  int screenrows;
  int screencols;

  int insert_mode;

  char filename[256];

} EditorState;

void editor_init(EditorState *E);
void editor_open(EditorState *E, const char *filename);
void editor_refresh_screen(EditorState *E);
void editor_process_key(EditorState *E);

#endif
