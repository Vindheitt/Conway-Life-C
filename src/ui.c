#include "all_src_files.h"

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
