#include "life_system.h"
#include "ui.h"

#include "area.h"
#include "file_io.h"
#include "game.h"
#include "utils.h"


status_t uiLogic(char* title, char* menuItems[], int menuCount, int *userChoose){
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
    uiLogic(title, menuItems, menuCount, &userChoose);

    clear();

    switch (userChoose) {
        case 0:
            startGame(area,config);
            break;
        case 1:
            optionsMenu(config);
            break;
        case 2:
            if(readLifeAreaUI(&area, config) == STATUS_OK)
                startGame(area,config);
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
//-----------------------------------------------------
status_t optionsMenu(options_t* config){
    int userChoose;
    if(!config)
        return STATUS_ERR_NULL_PTR;
    clear();
    do{
        printw("Choose options\n");
        do{
            printOptions(config);
            printw("Enter your choose: ");
            refresh();
        }while(enterInt(&userChoose));

        actionOptions(config, userChoose);
    }while(userChoose);
    clear();
    return STATUS_OK;
}
status_t actionOptions(options_t* config, int userChoose){
    int newValue;
    if(!config)
        return STATUS_ERR_NULL_PTR;
    switch (userChoose) {
        case 1:
            do{
                clear();
                printw("Choose type of rule:\n");
                printw("\t0)Outside = toroidal\n");
                printw("\t1)Outside = dead\n");
                printw("Enter your choose: ");
            }while(enterInt(&newValue) || (newValue != 1 && newValue !=0));
            config->rule = newValue;
            break;
        case 2:
            // do{
            //     clear();
            //     printw("Enter new value (ms, >=100): ");
            // }while(enterInt(&newValue) || (newValue < 100));
            config->waitTime = newValue;
            break;
        case 3:
            // chooseSize(config);

            break;
    }
    clear();
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
status_t enterNewOptionValue(options_t* config, size_t* value, size_t maxValue){
    if(!config || !value)
        return STATUS_ERR_NULL_PTR;

    while(value < 1 || (int)value > maxValue){
        clear();
        do{
            printw("Please enter new value (1..%d): ", maxValue);
            refresh();
        }while(enterSize(value));
        if(value < 1 || (int)value > maxValue){

            printw("Incorrect value. Please try again.\n");
        }
    }
    clear();

    return STATUS_OK;
}
void printOptions(options_t* config) {
    printw("\t1 - Change rules (now %s)\n", (config->rule) ? "locked" : "toroidal");
    printw("\t2 - Change wait time (now %d)\n", config->waitTime);
    printw("\t3 - Change area size (now %zux%zu)\n", config->rows, config->cols);
    printw("\t0 - Return\n");
}
