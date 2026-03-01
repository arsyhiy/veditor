#include <ncurses.h>

int main() {

    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    flushinp();

    while(1){
        printf("\033[2J\033[H");
        printf("hello world\n");
        napms(1);
    };
    endwin();
    return 0;
};
