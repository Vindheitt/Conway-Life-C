#include "life_system.h"
#include "ui.h"

#include "area.h"
#include "file_io.h"
#include "game.h"
#include "utils.h"


status_t menuDrawUI(char* title, char* menuItems[], int menuCount, int *userChoose){
    int ch = 0;
    int i;

    if(!title || !menuItems || !userChoose)
        return STATUS_ERR_NULL_PTR;

    while(ch != ENTER){
        printw("%s", title);
        for(i = 0; i < menuCount; i++){
            printw("\t");
            if(i == *userChoose)
                attron(A_REVERSE);
            printw("%s", menuItems[i]);
            if(i == *userChoose)
                attroff(A_REVERSE);
        }
        refresh();
        ch = tolower(getch());
        switch(ch){
            case 'k': case KEY_UP:
                *userChoose = (*userChoose <= 0) ? (menuCount - 1) : (*userChoose - 1);
                break;
            case 'j': case KEY_DOWN:
                *userChoose = (*userChoose >= menuCount-1) ? 0 : (*userChoose + 1);
                break;
        }
        clear();
    }
    return STATUS_OK;
}
status_t mainMenu(area_t *area, options_t* config){
    char* title = "Conway's game Life\n";
    char* menuItems[] = {
        "Start\n",
        "Options\n",
        "Read from file\n",
        "Exit\n",
    };
    int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int userChoose = 0;

    if(!config)
        return STATUS_ERR_NULL_PTR;

    clear();
    refresh();
    menuDrawUI(title, menuItems, menuCount, &userChoose);

    clear();

    switch (userChoose) {
        case 0:
            startGame(area, config);
            break;
        case 1:
            optionsMenu(config);
            break;
        case 2:
            if(readLifeAreaUI(&area, config) == STATUS_OK)
                startGame(area, config);
            break;
        case 3:
            return STATUS_EXIT;
    }
    return STATUS_OK;
}
status_t printArea(const area_t *area, int curY, int curX) {
    int x, y;
    size_t rows;
    size_t cols;

    if (!area)
        return STATUS_ERR_NULL_PTR;

    rows = area->rows;
    cols = area->cols;
    clear();
    for (y = 0; y < (int)rows; y++) {
        for (x = 0; x < (int)cols; x++) {
            if (curY >= 0 && y == curY && x == curX)
                attron(A_REVERSE | A_BOLD);
            mvprintw(y, x * 2, "%c ", getCell(area,y,x) ? '@' : '*');
            if (curY >= 0 && y == curY && x == curX)
                attroff(A_REVERSE | A_BOLD);
        }
    }
    refresh();
    return STATUS_OK;
}
status_t printSimulatuon(const area_t* area, int alive, int generation, int paused){
    size_t rows;

    if(!area)
        return STATUS_ERR_NULL_PTR;

    rows = area->rows;

    if(!paused){
        printArea(area, -1, -1);
        mvprintw(rows + 1, 0,
                "Generation: %d   Alive: %d   (q - quit, p - pause)",
                generation, alive);
        refresh();
    }
    else{
        mvprintw(LINES-2, 0, ">>> pause (p - continue, s - save) <<<");
        refresh();
    }
    return STATUS_OK;
}
status_t moveAndChange(area_t *area) {
    int curY = 0, curX = 0;
    int ch = 0;
    size_t rows;
    size_t cols;

    if (!area){
        //system("echo error_moveAndChange_null_ptr > logs.txt");
        return STATUS_ERR_NULL_PTR;
    }

    rows = area->rows;
    cols = area->cols;

    while (ch != ENTER) {
        printArea(area, curY, curX);
        mvprintw(rows + 1, 0,
                 "Coord: y=%d x=%d (h,j,k,l - move, e - change, Enter - start, s - save; q - quit)",
                 curY + 1, curX + 1);
        refresh();

        ch = tolower(getch());

        switch (ch) {
            case 'k': case KEY_UP:
                curY = (curY > 0) ? (curY - 1) : ((int)rows - 1);
                break;
            case 'j': case KEY_DOWN:
                curY = (curY < (int)rows - 1) ? (curY + 1) : 0;
                break;
            case 'h': case KEY_LEFT:
                curX = (curX > 0) ? (curX - 1) : ((int)cols - 1);
                break;
            case 'l': case KEY_RIGHT:
                curX = (curX < (int)cols - 1) ? (curX + 1) : 0;
                break;
            case 'e':
                setCell(area, curY, curX, !getCell(area, curY, curX));
                break;
            case 's':
                saveLifeAreaUI(area);
                break;
            case 'q':
                clear();
                return STATUS_EXIT;
            default:
                break;
        }
    }
    return STATUS_OK;
}
status_t saveLifeAreaUI(const area_t* area){
    char filename[256];
    int result;

    if (!area) {
        mvprintw(LINES-2, 0, "ERR. Area is NULL.\n");
        refresh();
        return STATUS_ERR_NULL_PTR;
    }
    nodelay(stdscr, FALSE);
    echo();
    curs_set(1);

    move(LINES-2, 0);
    clrtoeol();

    mvprintw(LINES-2, 0, "Enter filename to save: ");
    refresh();
    getnstr(filename, sizeof(filename)-1);

    move(LINES-2, 0);
    clrtoeol();

    result = saveLifeArea(area, filename);

    switch(result){
        case(STATUS_OK):
            printw("Data successfully saved to '%s'\n", filename);
            break;
        case(STATUS_ERR_FILE_OPEN):
            printw("Error opening file '%s' for writing..\n", filename);
            break;
    }
    refresh();
    noecho();
    curs_set(0);
    waitMs(SAVE_WAIT);
    return STATUS_OK;
}
status_t readLifeAreaUI(area_t **area, options_t* config){
    char filename[256];
    int result;

    clear();
    printw("Enter filename to read: ");
    refresh();

    echo();
    curs_set(1);
    getnstr(filename, sizeof(filename) - 1);

    result = readLifeArea(area, config, filename);

    switch(result){
        case(STATUS_ERR_FILE_OPEN):
            printw("Cannot open file '%s'\n", filename);
            break;
        case(STATUS_ERR_FILE_FORMAT):
            printw("Invalid file format: cannot read dimensions.\n");
            break;
    }

    refresh();
    noecho();
    curs_set(0);

    return STATUS_OK;
}
status_t optionsMenu(options_t* config){
    int userChoose = 0;
    int menuCount;

    char ruleInfo[50];
    char waitInfo[50];
    char sizeInfo[50];

    char* title = "Options\n";

    char* menuItems[] = {
        ruleInfo,
        waitInfo,
        sizeInfo,
        "Return"
    };

    menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    if(!config)
        return STATUS_ERR_NULL_PTR;

    do{
        snprintf(ruleInfo, sizeof(ruleInfo), "Change rules (now Outside %s)\n", config->rule == OUTSIDE_DEAD ? "dead" : "toroidal");
        snprintf(waitInfo, sizeof(waitInfo), "Change wait time (now %zu ms)\n", config->waitTime);
        snprintf(sizeInfo, sizeof(sizeInfo), "Change size (now %zux%zu)\n", config->rows, config->cols);

        menuDrawUI(title, menuItems, menuCount, &userChoose);
    }while(changeOptions(config, userChoose) != STATUS_EXIT);

    clear();
    return STATUS_OK;
}
//-----------------------------------------------------
status_t changeOptions(options_t* config, int userChoose){
    int newValue;
    if(!config)
        return STATUS_ERR_NULL_PTR;
    switch (userChoose) {
        case 0:
            changeRulesUI(config);
            break;
        case 1:
            do{
                clear();
                printw("Enter new value (ms, >=100): ");
            }while(enterInt(&newValue) || (newValue < 100));
            config->waitTime = newValue;
            break;
        case 2:
            chooseSize(config);
            break;
        case 3:
            return STATUS_EXIT;
    }
    return STATUS_OK;
}
status_t changeRulesUI(options_t* config){
    int userChoose = 0;
    int menuCount;

    char* title = "Choose type of rule:\n";

    char* menuItems[] = {
        "Outside = dead\n",
        "Outside = toroidal\n"
    };
    if(!config)
        return STATUS_ERR_NULL_PTR;

    menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    clear();

    menuDrawUI(title, menuItems, menuCount, &userChoose);

    config->rule = userChoose;

    return STATUS_OK;
}
status_t chooseSize(options_t* config){
    size_t rows = 0;
    size_t cols = 0;

    if(!config)
        return STATUS_ERR_NULL_PTR;

    clear();
    while(rows < 1 || (int)rows > LINES - 2){
        do{
            printw("Please enter quantity of rows (1..%d): ", LINES - 2);
            refresh();
        }while(enterSize(&rows));
        if(rows < 1 || (int)rows > LINES - 2){
            clear();
            printw("Incorrect value. Please try again.\n");
        }
    }
    clear();
    while(cols < 1 || (int)cols > (COLS/2) - 1){

        do{
            printw("Please enter quantity of columns (1..%d): ", (COLS/2) - 1);
            refresh();
        }while(enterSize(&cols));
        if(cols < 1 || (int)cols > (COLS/2) - 1){
            clear();
            printw("Incorrect value. Please try again.\n");
        }
    }
    config->rows = rows;
    config->cols = cols;

    return STATUS_OK;
}
