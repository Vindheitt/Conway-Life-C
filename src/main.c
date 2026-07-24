#include "all_src_files.h"

int main(void){
    int userChoose;
    options_t *config = makeOptions();

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    printw("Hello there!\n");
    do{
        do{
            printActions();
            printw("Enter your choose: ");
            refresh();
        }while(enterInt(&userChoose));

        action(userChoose, config);
    }while(userChoose);

    free(config);
    endwin();
    printf("See you soon!\n");
    return 0;
}
