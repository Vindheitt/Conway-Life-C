#include "all_src_files.h"

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
char getCell(const area_t *area, int y, int x) {
    return area->matrix[y * area->cols + x];
}
void setCell(area_t *area, int y, int x, char val) {
    area->matrix[y * area->cols + x] = val;
}
