#include "file.h"

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
