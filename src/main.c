#include "all_src_files.h"

int main(void){
    options_t *config = makeOptions();
    area_t *area = NULL;

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    action(area, config);

    destroyData(&area);
    free(config);

    endwin();
    return 0;
}
