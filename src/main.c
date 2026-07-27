#include "life_system.h"
#include "area.h"
// #include "game.h"
// #include "file_io.h"
// #include "utils.h"
#include "ui.h"

status_t makeOptions(options_t**);

int main(void){
    options_t *config;
    area_t *area = NULL;

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    makeOptions(&config);
    while(mainMenu(area, config) != STATUS_EXIT);

    destroyArea(&area);
    free(config);

    endwin();
    return 0;
}

status_t makeOptions(options_t** config){
    *config = malloc(sizeof(options_t));
    if (!config)
        return STATUS_ERR_MEMORY;
    (*config)->rule = OUTSIDE_DEAD;
    (*config)->waitTime = DEFAULT_WAIT;

    (*config)->cols = DEFAULT_WIDTH;
    (*config)->rows = DEFAULT_HEIGHT;
    return STATUS_OK;
}
