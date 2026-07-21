#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef _WIN32
    #include <curses.h>
    #include <windows.h>
#else
    #include <ncurses.h>
    #include <unistd.h>
#endif

#define TRUE 1
#define FALSE 0

#define OUTSIDE_DEAD 1
#define OUTSIDE_TOROIDAL 0

#define DEFAULT_HEIGHT 20
#define DEFAULT_WIDTH  40

#define DEFAULT_WAIT 100
#define SAVE_WAIT 2000

//#define swap(t, x, y) do{t temp = x; x = y; y = temp;} while(0)
#define swap(t, x, y) {t temp = x; x = y; y = temp;}

typedef struct Area{
    size_t cols;
    size_t rows;

    char *matrix;
}area_t;

typedef struct Options{
    int rule;
    /*int neighbors;*/
    int waitTime;

    size_t cols;
    size_t rows;
}options_t;

int readLifeArea(options_t*);
int action(int, options_t*);
int enterInt(int*);
int enterSize(size_t* n);
int waitMs(int);

int startGame(area_t*,options_t*);
int startSimulation(area_t*,options_t*);
int moveAndChange(area_t*);
int countOfAlive(const area_t*);
int saveLifeArea(const area_t*);
int printArea(const area_t*, int, int);
int checkCell(const area_t*, int, int, options_t*);

int nextGeneration(area_t*, area_t*, options_t*);

int actionOptions(options_t*, int);
int changeOptions(options_t*);
int chooseSize(options_t*);

void destroyData(area_t**);
void printOptions(options_t*);
void printActions(void);
void endOfProgramm(void);
void defaultChoose(void);
void setCell(area_t*, int, int, char);

char getCell(const area_t*, int, int);

area_t* createArea(size_t, size_t);
options_t* makeOptions();

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
    if(!config)
        return -1;
    switch (userChoose) {
        case 1:
            startGame(NULL,config);
            break;
        case 2:
            changeOptions(config);
            break;
        case 3:
            readLifeArea(config);
            break;
        case 0:

            break;
        default:
            defaultChoose();
            break;
    }
    return 0;
}
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
int startGame(area_t *area, options_t* config){
    if(!area)
        area = createArea(config->rows, config->cols);

    if(!moveAndChange(area))
        startSimulation(area, config);
    destroyData(&area);
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

    while (ch != 10) {
        printArea(area, curY, curX);
        mvprintw(rows + 1, 0,
                 "Coord: y=%d x=%d (h,j,k,l - move, e - change, Enter - start, s - save; q - quit)",
                 curY + 1, curX + 1);
        refresh();

        ch = tolower(getch());

        switch (ch) {
            case 'k':
            case KEY_UP:
                if (curY > 0) curY--;
                else curY = (int)rows - 1;
                break;
            case 'j':
            case KEY_DOWN:
                if (curY < (int)rows - 1) curY++;
                else curY = 0;
                break;
            case 'h':
            case KEY_LEFT:
                if (curX > 0) curX--;
                else curX = (int)cols - 1;
                break;
            case 'l':
            case KEY_RIGHT:
                if (curX < (int)cols - 1) curX++;
                else curX = 0;
                break;
            case 'e':
                setCell(area, curY, curX, !getCell(area, curY, curX));
                break;
            case 's':
                saveLifeArea(area);
                break;
            case 'q':
                clear();
                printw("What can I do for you?\n");
                return 1;
            default:
                break;
        }
    }
    return 0;
}
int readLifeArea(options_t* config){
    char filename[256];
    size_t rows, cols;
    size_t y, x;
    int c;
    FILE *file;

    clear();
    printw("Enter filename to read: ");
    refresh();

    echo();
    curs_set(1);
    getnstr(filename, sizeof(filename) - 1);

    file = fopen(filename, "r");
    if (!file) {
        printw("Cannot open file '%s'\n", filename);
        refresh();
        noecho();
        curs_set(0);
        return -1;
    }

    if (fscanf(file, "%zu %zu", &rows, &cols) != 2) {
        printw("Invalid file format: cannot read dimensions.\n");
        refresh();
        noecho();
        curs_set(0);
        fclose(file);
        return -1;
    }

    config->rows = rows;
    config->cols = cols;

    area_t* newArea = createArea(rows, cols);
    if (!newArea) {
        noecho();
        curs_set(0);
        fclose(file);
        return -1;
    }

    x = 0;
    y = 0;
    while ((c = fgetc(file)) != EOF && y < rows) {
        if (c == '0' || c == '1') {
            setCell(newArea, y, x, (c == '1'));
            x++;
            if (x >= cols) {
                x = 0;
                y++;
            }
        }
    }
    noecho();
    curs_set(0);

    fclose(file);
    refresh();
    startGame(newArea, config);
    return 0;
}
int saveLifeArea(const area_t *area) {
    char filename[256];
    size_t y,x;
    size_t rows;
    size_t cols;
    FILE *file;

    if (!area) {
        mvprintw(LINES-2, 0, "Err. Area is NULL.\n");
        refresh();
        return -1;
    }
    rows = area->rows;
    cols = area->cols;

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

    file = fopen(filename, "w");
    if (!file) {
        printw("Error opening file '%s' for writing..\n", filename);
        refresh();
        return -1;
    }

    fprintf(file, "%zu %zu\n", rows, cols);

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++)
            fputc(getCell(area, y,x) ? '1' : '0', file);
        fputc('\n', file);
    }

    fclose(file);
    printw("Data successfully saved to '%s'\n", filename);
    refresh();

    noecho();
    curs_set(0);
    waitMs(SAVE_WAIT);
    return 0;
}
int startSimulation(area_t *area,options_t* config) {
    int paused = 0;
    int generation = 0;
    int alive = 0;
    int ch;

    size_t rows;
    size_t cols;

    if (!area)
        return -1;

    rows = area->rows;
    cols = area->cols;

    area_t *tempArea = createArea(rows, cols);
    if (!tempArea)
        return -1;

    while (TRUE) {
        nodelay(stdscr, TRUE);
        ch = tolower(getch());
        if (ch == 'q')
            break;
        else if (ch == 'p')
            paused = !paused;

        if (!paused) {
            nextGeneration(tempArea, area, config);
            generation++;
            alive = countOfAlive(area);
            printArea(area, -1, -1);
            mvprintw(rows + 1, 0,
                     "Generation: %d   Alive: %d   (q - quit, p - pause)",
                     generation, alive);
            refresh();
            waitMs(config->waitTime);
        }
        else {
            mvprintw(LINES-2, 0, ">>> pause (p - continue, s - save) <<<");
            refresh();
            if (ch == 's')
                saveLifeArea(area);
        }
    }
    nodelay(stdscr, FALSE);
    destroyData(&tempArea);
    clear();
    printw("What can I do for you?\n");
    return 0;
}
int countOfAlive(const area_t *area) {
    int x,y;
    int alive = 0;
    size_t rows;
    size_t cols;

    if (!area)
        return -1;

    rows = area->rows;
    cols = area->cols;
    for (y = 0; y < (int)rows; y++)
        for (x = 0; x < (int)cols; x++)
            alive += getCell(area,y,x);
    return alive;
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
int nextGeneration(area_t *tempArea, area_t *area, options_t* config) {
    int x, y;
    size_t rows;
    size_t cols;

    if (!tempArea || !area || !config)
        return -1;
    rows = area->rows;
    cols = area->cols;
    for (y = 0; y < (int)rows; y++)
        for (x = 0; x < (int)cols; x++)
            setCell(tempArea, y, x, checkCell(area, y, x, config));
    swap(char*, area->matrix, tempArea->matrix);
    return 0;
}
int checkCell(const area_t *area, int y, int x, options_t* config) {
    int neighbors = 0;
    int dx, dy;
    int ny, nx;

    size_t rows;
    size_t cols;

    if (!area)
        return -1;

    rows = area->rows;
    cols = area->cols;

    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0)
                continue;
            if(config->rule == OUTSIDE_DEAD){
                ny = y + dy;
                nx = x + dx;
                if(ny >= (int)rows || ny < 0 || nx >= (int)cols || nx < 0)
                    continue;
            }
            else{
                ny = y + dy + (int)rows;
                ny %= rows;
                nx = x + dx + (int)cols;
                nx %= cols;
            }

            if (getCell(area, ny, nx) == 1)
                neighbors++;
        }
    }
    if (getCell(area,y,x))
        return (neighbors == 2 || neighbors == 3) ? 1 : 0;
    else
        return (neighbors == 3) ? 1 : 0;
}
int waitMs(int ms) {
    if (ms <= 0) return -1;
    #ifdef WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
    return 0;
}
int enterInt(int *n) {
    char buf[32];

    if (!n)
        return -1;

    echo();
    curs_set(1);
    while (TRUE) {
        getnstr(buf, sizeof(buf)-1);
        if (sscanf(buf, "%d", n) == 1)
            break;
        clear();
        printw("Something went wrong with enter int..\n");
        refresh();
        clrtoeol();
        refresh();
        noecho();
        curs_set(0);
        return 1;
    }
    curs_set(0);
    noecho();
    return 0;
}
int enterSize(size_t *n) {
    char buf[32];

    if (!n)
        return -1;

    echo();
    curs_set(1);
    while (TRUE) {
        getnstr(buf, sizeof(buf)-1);
        if (sscanf(buf, "%zu", n) == 1)
            break;
        clear();
        printw("Something went wrong with enter int..\n");
        refresh();
        clrtoeol();
        refresh();
        noecho();
        curs_set(0);
        return 1;
    }
    curs_set(0);
    noecho();
    return 0;
}
void printOptions(options_t* config) {
    printw("\t1 - Change rules (now Outside %s)\n", (config->rule) ? "dead" : "toroidal");
    printw("\t2 - Change wait time (now %d)\n", config->waitTime);
    printw("\t3 - Change size (now %zux%zu)\n", config->rows,config->cols);
    printw("\t0 - Return\n");
}
void printActions(void) {
    printw("\t1 - Start\n");
    printw("\t2 - Options\n");
    printw("\t3 - Read from file\n");
    printw("\t0 - Exit\n");
}
void defaultChoose(void) {
    clear();
    printw("Something went wrong with choose...\n");
    refresh();
}
void destroyData(area_t **area) {
    if (!area || !*area)
        return;
    free((*area)->matrix);
    free(*area);
    *area = NULL;
}
area_t* createArea(size_t rows, size_t cols) {
    area_t *area = malloc(sizeof(area_t));
    if(!area)
        return NULL;
    area->rows = rows;
    area->cols = cols;
    area->matrix = calloc(rows * cols, sizeof(char));
    if(!area->matrix){
        free(area);
        return NULL;
    }
    return area;
}
options_t* makeOptions(){
    options_t* config = malloc(sizeof(options_t));
    if (!config)
        return NULL;
    config->rule = OUTSIDE_DEAD;
    config->waitTime = DEFAULT_WAIT;

    config->cols = DEFAULT_WIDTH;
    config->rows = DEFAULT_HEIGHT;
    return config;
}
char getCell(const area_t *area, int y, int x) {
    return area->matrix[y * area->cols + x];
}
void setCell(area_t *area, int y, int x, char val) {
    area->matrix[y * area->cols + x] = val;
}
