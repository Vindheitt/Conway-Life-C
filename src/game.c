#include "all_src_files.h"

int startGame(area_t *area, options_t* config){
    if(!area)
        area = createArea(config->rows, config->cols);

    if(!moveAndChange(area))
        startSimulation(area, config);
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
