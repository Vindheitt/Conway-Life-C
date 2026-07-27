#include "all_src_files.h"

status_t startGame(area_t* area, options_t* config){
    if(!area)
        createArea(&area, config->rows, config->cols);
    if(!area)
        return STATUS_ERR_NULL_PTR;
    if(moveAndChange(area) == STATUS_OK)
        startSimulation(area, config);
    return STATUS_OK;
}
status_t startSimulation(area_t* area, options_t* config) {
    int paused = 0;
    int generation = 0;
    int alive;
    int ch;

    size_t rows;
    size_t cols;

    area_t *tempArea;

    if (!area)
        return STATUS_ERR_NULL_PTR;

    rows = area->rows;
    cols = area->cols;

    createArea(&tempArea, rows, cols);

    if (!tempArea)
        return STATUS_ERR_NULL_PTR;

    while (TRUE) {
        printSimulatuon(area, alive, generation, paused);

        nodelay(stdscr, TRUE);
        ch = tolower(getch());
        if (ch == 'q')
            break;
        else if (ch == 'p')
            paused = !paused;

        if (!paused) {
            nextGeneration(tempArea, area, config);
            generation++;
            countOfAlive(area, &alive);

            waitMs(config->waitTime);
        }
        else {

            if (ch == 's')
                saveLifeArea(area);
        }
    }
    nodelay(stdscr, FALSE);
    destroyArea(&tempArea);
    //clear();
    return STATUS_OK;
}
status_t countOfAlive(const area_t* area, int* alive) {
    int x,y;
    size_t rows;
    size_t cols;

    if (!area)
        return STATUS_ERR_NULL_PTR;

    *alive = 0;

    rows = area->rows;
    cols = area->cols;
    for (y = 0; y < (int)rows; y++)
        for (x = 0; x < (int)cols; x++)
            *alive += getCell(area,y,x);
    return STATUS_OK;
}
status_t nextGeneration(area_t* tempArea, area_t* area, options_t* config) {
    int x, y;
    int state;
    size_t rows;
    size_t cols;

    if (!tempArea || !area || !config)
        return STATUS_ERR_NULL_PTR;
    rows = area->rows;
    cols = area->cols;
    for (y = 0; y < (int)rows; y++)
        for (x = 0; x < (int)cols; x++)
            if(checkCell(area, y, x, &state, config) == STATUS_OK)
                setCell(tempArea, y, x, state);
    swap(char*, area->matrix, tempArea->matrix);
    return STATUS_OK;
}
status_t checkCell(const area_t* area, int y, int x, int* state, options_t* config) {
    int neighbors = 0;
    int dx, dy;
    int ny, nx;

    size_t rows;
    size_t cols;

    if (!area)
        return STATUS_ERR_NULL_PTR;

    rows = area->rows;
    cols = area->cols;

    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0)
                continue;
            if(config->rule == OUTSIDE_DEAD){
                ny = y + dy;
                nx = x + dx;
                if(ny >= (int)rows || nx >= (int)cols || ny < 0 || nx < 0)
                    continue;
            }
            else{
                ny = y + dy + (int)rows;
                ny %= rows;
                nx = x + dx + (int)cols;
                nx %= cols;
            }
            neighbors += getCell(area, ny, nx);
        }
    }
    if (getCell(area,y,x))
        *state = (neighbors == 2 || neighbors == 3) ? 1 : 0;
    else
        *state = (neighbors == 3) ? 1 : 0;

    return STATUS_OK;
}
