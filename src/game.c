#include "all_src_files.h"

int startGame(area_t* area, options_t* config){
    if(!area)
        createArea(&area, config->rows, config->cols);
    if(!area)
        return -1;
    if(moveAndChange(area) == STATUS_OK)
        startSimulation(area, config);
    return 0;
}
int startSimulation(area_t* area, options_t* config) {
    int paused = 0;
    int generation = 0;
    int alive;
    int ch;

    size_t rows;
    size_t cols;

    area_t *tempArea;

    if (!area)
        return -1;

    rows = area->rows;
    cols = area->cols;

    createArea(&tempArea, rows, cols);

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
            countOfAlive(area, &alive);
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
    destroyArea(&tempArea);
    clear();
    return 0;
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
    size_t rows;
    size_t cols;

    if (!tempArea || !area || !config)
        return STATUS_ERR_NULL_PTR;
    rows = area->rows;
    cols = area->cols;
    for (y = 0; y < (int)rows; y++)
        for (x = 0; x < (int)cols; x++)
            newCellStatus(area, tempArea, y, x, config);
            //setCell(tempArea, y, x, checkCell(area, y, x, config));
    swap(char*, area->matrix, tempArea->matrix);
    return STATUS_OK;
}
// int checkCell(const area_t* area, int y, int x, options_t* config) {
//     int neighbors = 0;
//     int dx, dy;
//     int ny, nx;
//
//     size_t rows;
//     size_t cols;
//
//     if (!area)
//         return -1;
//
//     rows = area->rows;
//     cols = area->cols;
//
//     for (dy = -1; dy <= 1; dy++) {
//         for (dx = -1; dx <= 1; dx++) {
//             if (dy == 0 && dx == 0)
//                 continue;
//             if(config->rule == OUTSIDE_DEAD){
//                 ny = y + dy;
//                 nx = x + dx;
//                 if(ny >= (int)rows || nx >= (int)cols || ny < 0 || nx < 0)
//                     continue;
//             }
//             else{
//                 ny = y + dy + (int)rows;
//                 ny %= rows;
//                 nx = x + dx + (int)cols;
//                 nx %= cols;
//             }
//             neighbors += getCell(area, ny, nx);
//         }
//     }
//     if (getCell(area,y,x))
//         return (neighbors == 2 || neighbors == 3) ? 1 : 0;
//     else
//         return (neighbors == 3) ? 1 : 0;
// }
status_t newCellStatus(area_t* area, area_t* tempArea, int y, int x, options_t* config) {
    int neighbors = 0;
    int dx, dy;
    int ny, nx;
    int cellStatus;

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
        cellStatus = ((neighbors == 2 || neighbors == 3) ? 1 : 0);
    else
        cellStatus = ((neighbors == 3) ? 1 : 0);

    setCell(tempArea, y, x, cellStatus);

    return STATUS_OK;
}
