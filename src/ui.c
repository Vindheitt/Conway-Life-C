#include "all_src_files.h"

int changeOptions(options_t* config){
    int userChoose;
    if(!config)
        return -1;
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
    printw("What can I do for you?\n");
    return 0;
}
int actionOptions(options_t* config, int userChoose){
    int newValue;
    if(!config)
        return -1;
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
            do{
                clear();
                printw("Enter new value (ms, >=100): ");
            }while(enterInt(&newValue) || (newValue < 100));
            config->waitTime = newValue;

            break;
        case 3:
            chooseSize(config);
            break;
        case 0:

            break;
        default:
            defaultChoose();
            break;
    }
    clear();
    return 0;
}
int chooseSize(options_t* config){
    size_t rows = 0;
    size_t cols = 0;

    if(!config)
        return -1;

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

    return 0;
}
int action(area_t *area, options_t* config){
    int userChoose = 0;
    int ch = 0;

    while(ch != ENTER){
        clear();
        refresh();
        printw("Conway game \"Life\"\n");
        printActions(userChoose);
        ch = tolower(getch());
        switch(ch){
            case 'k': case KEY_UP:
                userChoose = (userChoose <= 0) ? 3 : (userChoose - 1);
                break;
            case 'j': case KEY_DOWN:
                userChoose = (userChoose >= 3) ? 0 : (userChoose + 1);
                break;
        }
    }

    // do{
    //     do{
    //         printActions();
    //         printw("Enter your choose: ");
    //         refresh();
    //     }while(enterInt(&userChoose));
    //
    //     action(userChoose, config);
    // }while(userChoose);

    clear();
    if(!config)
        return -1;

    switch (userChoose) {
        case 0:
            startGame(area,config);
            break;
        case 1:
            changeOptions(config);
            break;
        case 2:
            if(readLifeArea(&area, config) == STATUS_OK)
                startGame(area,config);
            break;
        default:
            defaultChoose();
            break;
    }
    return 0;
}
void printOptions(options_t* config) {
    printw("\t1 - Change rules (now Outside %s)\n", (config->rule) ? "dead" : "toroidal");
    printw("\t2 - Change wait time (now %d)\n", config->waitTime);
    printw("\t3 - Change size (now %zux%zu)\n", config->rows, config->cols);
    printw("\t0 - Return\n");
}
void printActions(int userChoose) {
    int i = 0;
    char* menuTags[] = {
        "\t1 - Start\n",
        "\t2 - Options\n",
        "\t3 - Read from file\n",
        "\t0 - Exit\n"
    };
    for(i = 0; i < 4; i++){
        if(i == userChoose)
            attron(A_REVERSE);
        printw("%s", menuTags[i]);
        if(i == userChoose)
            attroff(A_REVERSE);
    }

    // printw("\t1 - Start\n");
    // printw("\t2 - Options\n");
    // printw("\t3 - Read from file\n");
    // printw("\t0 - Exit\n");
}
void defaultChoose(void) {
    clear();
    printw("Something went wrong with choose...\n");
    refresh();
}
int printArea(const area_t *area, int curY, int curX) {
    int x, y;
    size_t rows;
    size_t cols;

    if (!area)
        return -1;

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
    return 0;
}
int moveAndChange(area_t *area) {
    int curY = 0, curX = 0;
    int ch = 0;
    size_t rows;
    size_t cols;

    if (!area)
        return -1;

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
                saveLifeArea(area);
                break;
            case 'q':
                clear();
                return 1;
            default:
                break;
        }
    }
    return 0;
}
