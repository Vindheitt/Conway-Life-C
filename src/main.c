#include "all_src_files.h"

int action(int, options_t*);

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
int action(int userChoose, options_t* config){
    clear();
    area_t *area = NULL;
    if(!config)
        return -1;
    switch (userChoose) {
        case 1:
            startGame(area,config);
            break;
        case 2:
            changeOptions(config);
            break;
        case 3:
            readLifeArea(&area, config);
            break;
        case 0:

            break;
        default:
            defaultChoose();
            break;
    }
    return 0;
}
