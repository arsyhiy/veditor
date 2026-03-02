// #include <stdio.h>
//
// int main(int argc, char *argv[]) {
//
//   if (argc > 1)
//     //     editor_open(&E, argv[1]);
//     printf("do something");
//
//   while (1) {
//     //     editor_refresh_screen(&E);
//     //     editor_process_key(&E);
//   }
// }


#include "editor.h"

int main(int argc, char *argv[]) {
  EditorState E;

  editor_init(&E);

  if (argc > 1)
    editor_open(&E, argv[1]);

  while (1) {
    editor_refresh_screen(&E);
    editor_process_key(&E);
  }
}
