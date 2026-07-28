#include "life_system.h"
#include "game.h"

#include "area.h"
#include "ui.h"
#include "utils.h"

status_t startGame(area_t** area, options_t* config){
    //status_t status;
    if(!area)
        return STATUS_ERR_NULL_PTR;
    if(!(*area))
        createArea(area, config->rows, config->cols);
    // if(status != STATUS_OK) status =
    //     return status;
    if(!(*area))
        return STATUS_ERR_NULL_PTR;

    if(moveAndChange(area) == STATUS_OK)
        startSimulation(area, config);
    return STATUS_OK;
}
status_t startSimulation(area_t** area, options_t* config) {
    int paused = 0;
    int generation = 0;
    int alive;
    int ch;

    size_t rows;
    size_t cols;

    area_t *tempArea;

    if (!area || !(*area))
        return STATUS_ERR_NULL_PTR;

    rows = (*area)->rows;
    cols = (*area)->cols;

    if (createArea(&tempArea, rows, cols) != STATUS_OK)
        return STATUS_ERR_NULL_PTR;

    nodelay(stdscr, TRUE);
    while (TRUE) {
        ch = tolower(getch());
        if (ch == 'q')
            break;
        if (ch == 'p')
            paused = !paused;
        if (ch == 's' && paused)
            saveLifeAreaUI((*area));
        if (!paused) {
            nextGeneration(tempArea, (*area), config);
            generation++;
            countOfAlive((*area), &alive);
        }
        printSimulation((*area), alive, generation, paused);
        waitMs(config->waitTime);
    }
    nodelay(stdscr, FALSE);
    destroyArea(&tempArea);
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
