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
        clear();
        printw("%s\n", title);
        for(i = 0; i < menuCount; i++){
            printw("\t");
            if(i == *userChoose)
                attron(A_REVERSE);
            printw("%s\n", menuItems[i]);
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

    }
    return STATUS_OK;
}
status_t mainMenu(area_t** area, options_t* config){
    char* title = "Conway's game Life";
    char* menuItems[] = {
        "New game",
        "Continue",
        "Options",
        "Read from file",
        "Exit",
    };
    int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int userChoose = 0;

    status_t status;

    if(!config)
        return STATUS_ERR_NULL_PTR;
    do{
        clear();
        refresh();
        menuDrawUI(title, menuItems, menuCount, &userChoose);

        status = STATUS_OK;

        switch (userChoose) {
            case 0:
                if (area && *area)
                    destroyArea(area);
                CHECK_PRINT(startGame(area, config));
                break;
            case 1:
                CHECK_PRINT(startGame(area, config));
                break;
            case 2:
                optionsMenu(config);
                break;
            case 3:
                status = readLifeAreaUI(area, config);
                if(status == STATUS_OK)
                    startGame(area, config);
                break;
            case 4:
                status = STATUS_EXIT;
        }
        CHECK_PRINT(status);
    }while(status != STATUS_EXIT);

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
status_t printSimulation(const area_t* area, int alive, int generation, int paused){
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
status_t moveAndChange(area_t** area) {
    int curY = 0, curX = 0;
    int ch = 0;
    size_t rows;
    size_t cols;

    if (!area || !(*area))
        return STATUS_ERR_NULL_PTR;

    rows = (*area)->rows;
    cols = (*area)->cols;

    while (ch != ENTER) {
        printArea((*area), curY, curX);
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
                setCell((*area), curY, curX, !getCell((*area), curY, curX));
                break;
            case 's':
                saveLifeAreaUI((*area));
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
    char path[512];
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
    snprintf(path, sizeof(path), "areas/%s", filename);
    result = saveLifeArea(area, path);

    switch(result){
        case(STATUS_OK):
            printw("Data successfully saved to '%s'\n", path);
            break;
        case(STATUS_ERR_FILE_OPEN):
            printw("Error opening file '%s' for writing..\n", path);
            break;
    }
    refresh();
    noecho();
    curs_set(0);
    waitMs(SAVE_WAIT);
    return STATUS_OK;
}
status_t readLifeAreaUI(area_t** area, options_t* config) {
    DIR* dir;
    struct dirent *entry;

    int fileCount = 0;
    int userChoose = 0;
    int i;

    status_t status;

    char fullPath[512];
    char* fileList[MAX_FILES];
    char* title = "Select file to load:";

    dir = opendir("areas");
    if (!dir) {
        printw("Error: cannot open areas/ directory.\n");
        return STATUS_ERR_FILE_OPEN;
    }

    while ((entry = readdir(dir)) != NULL && fileCount < 255) {
        if (entry->d_name[0] == '.')
            continue;
        fileList[fileCount++] = strdup(entry->d_name);
    }
    closedir(dir);

    if (fileCount == 0) {
        printw("No files found in areas/ directory.\n");
        return STATUS_ERR_FILE_OPEN;
    }

    fileList[fileCount] = "Return";
    fileCount++;

    clear();

    if (menuDrawUI(title, fileList, fileCount, &userChoose) != STATUS_OK) {
        for (i = 0; i < fileCount; i++)
            free(fileList[i]);
        return STATUS_ERR_UNKNOWN;
    }

    if (userChoose == fileCount - 1){
        for (i = 0; i < fileCount - 1; i++)
            free(fileList[i]);
        return STATUS_EXIT;
    }


    snprintf(fullPath, sizeof(fullPath), "areas/%s", fileList[userChoose]);

    status = readLifeArea(area, config, fullPath);

    switch(status){
        case(STATUS_ERR_FILE_OPEN):
            printw("Cannot open file '%s'\n", fullPath);
            refresh();
            waitMs(SAVE_WAIT);
            break;
        case(STATUS_ERR_FILE_FORMAT):
            printw("Invalid file format: cannot read dimensions.\n");
            refresh();
            waitMs(SAVE_WAIT);
            break;
        default:
            CHECK_PRINT(status);
            break;
    }

    for (i = 0; i < fileCount - 1; i++)
        free(fileList[i]);

    return STATUS_OK;
}
status_t optionsMenu(options_t* config){
    int userChoose = 0;
    int menuCount;

    char ruleInfo[50];
    char waitInfo[50];
    char sizeInfo[50];

    char* title = "Options";

    char* menuItems[] = {
        ruleInfo,
        waitInfo,
        sizeInfo,
        "Return"
    };

    if(!config)
        return STATUS_ERR_NULL_PTR;

    menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    do{
        snprintf(ruleInfo, sizeof(ruleInfo), "Change rules (now Outside %s)", config->rule == OUTSIDE_DEAD ? "dead" : "toroidal");
        snprintf(waitInfo, sizeof(waitInfo), "Change wait time (now %zu ms)", config->waitTime);
        snprintf(sizeInfo, sizeof(sizeInfo), "Change size (now %zux%zu)", config->rows, config->cols);

        clear();
        menuDrawUI(title, menuItems, menuCount, &userChoose);
    }while(changeOptions(config, userChoose) != STATUS_EXIT);

    return STATUS_OK;
}
status_t changeOptions(options_t* config, int userChoose){
    if(!config)
        return STATUS_ERR_NULL_PTR;
    switch (userChoose) {
        case 0:
            changeRulesUI(config);
            break;
        case 1:
            changeWaitTimeUI(config);
            break;
        case 2:
            changeSizeUI(config);
            break;
        case 3:
            return STATUS_EXIT;
    }
    return STATUS_OK;
}
status_t changeRulesUI(options_t* config){
    int userChoose = 0;
    int menuCount;

    char* title = "Choose type of rule:";

    char* menuItems[] = {
        "Outside = dead",
        "Outside = toroidal"
    };
    if(!config)
        return STATUS_ERR_NULL_PTR;

    menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    clear();

    menuDrawUI(title, menuItems, menuCount, &userChoose);

    config->rule = userChoose;

    return STATUS_OK;
}
status_t changeWaitTimeUI(options_t* config){
    size_t waitTime;
    status_t result;

    while(TRUE){
        printw("Enter new wait-time (ms, >= 10): ");
        refresh();
        result = enterSize(&waitTime);
        clear();

        if(result == STATUS_ERR_INVALID_INPUT){
            printw("Something went wrong with enter number...\n");
        }
        else if(waitTime < 10)
            printw("Incorrect value. Please try again.\n");
        else
            break;
    }
    clear();

    config->waitTime = waitTime;
    return STATUS_OK;
}
status_t changeSizeUI(options_t* config){
    size_t rows = 0;
    size_t cols = 0;

    status_t result;

    if(!config)
        return STATUS_ERR_NULL_PTR;

    clear();

    while(TRUE){
        printw("Please enter quantity of rows (1..%d): ", LINES - 2);
        refresh();
        result = enterSize(&rows);
        clear();

        if(result == STATUS_ERR_INVALID_INPUT){
            printw("Something went wrong with enter number...\n");
        }
        else if(rows < 1 || (int)rows > LINES - 2)
            printw("Incorrect value. Please try again.\n");
        else
            break;
    }
    clear();
    while(TRUE){
        printw("Please enter quantity of columns (1..%d): ", (COLS/2) - 1);
        refresh();
        result = enterSize(&cols);
        clear();

        if(result == STATUS_ERR_INVALID_INPUT){
            printw("Something went wrong with enter number...\n");
        }
        else if(cols < 1 || (int)cols > (COLS/2) - 1)
            printw("Incorrect value. Please try again.\n");
        else
            break;
    }
    config->rows = rows;
    config->cols = cols;

    return STATUS_OK;
}
status_t printError(status_t error){
    if(error == STATUS_OK || error == STATUS_EXIT)
        return STATUS_OK;
    move(LINES-2, 0);
    clrtoeol();
    mvprintw(LINES-2, 0, "%s", statusToString(error));
    refresh();
    getch();
    return STATUS_OK;
}
const char* statusToString(status_t status) {
    static const char* names[] = {
        "STATUS_OK",
        "STATUS_EXIT",
        "STATUS_ERR_NULL_PTR",
        "STATUS_ERR_MEMORY",
        "STATUS_ERR_FILE_OPEN",
        "STATUS_ERR_FILE_FORMAT",
        "STATUS_ERR_INVALID_INPUT",
        "STATUS_ERR_EMPTY_STR"
    };
    if (status < 0 || status >= sizeof(names)/sizeof(names[0]))
        return "UNKNOWN";
    return names[status];
}
