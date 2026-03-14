#include "editor.h"
#include "file.h"
#include "display.h"
#include "input.h"

int main(int argc, char *argv[]) { 
  Editor E = {0}; // NOTE: пересмотреть способ инитиализации E. 

  init(&E);

  if (argc > 1)
    editor_open(&E, argv[1]);

  while (1) {
    // NOTE: можно перенести это в refresh_editor(&E);
    editor_refresh_screen(&E);
    editor_process_key(&E);
  }

  shutdown(&E);
  return 0;
}
