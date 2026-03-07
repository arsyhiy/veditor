#include "editor.h"
#include "file.h"
#include "display.h"
#include "input.h"

int main(int argc, char *argv[]) {
  Editor E = {0};

  init(&E);

  if (argc > 1)
    editor_open(&E, argv[1]);

  while (1) {
    editor_refresh_screen(&E);
    editor_process_key(&E);
  }
}
