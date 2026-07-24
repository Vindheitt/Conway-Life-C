#include "all_src_files.h"

status_t readLifeArea(area_t **area, options_t* config){
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
        return STATUS_ERR_FILE_OPEN;
    }

    if (fscanf(file, "%zu %zu", &rows, &cols) != 2) {
        printw("Invalid file format: cannot read dimensions.\n");
        refresh();
        noecho();
        curs_set(0);
        fclose(file);
        return STATUS_ERR_FILE_FORMAT;
    }

    config->rows = rows;
    config->cols = cols;

    area_t* newArea = createArea(rows, cols);
    if (!newArea) {
        noecho();
        curs_set(0);
        fclose(file);
        return STATUS_ERR_NULL_PTR;
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
    *area = newArea;

    return STATUS_OK;
}
status_t saveLifeArea(const area_t *area) {
    char filename[256];
    size_t y,x;
    size_t rows;
    size_t cols;
    FILE *file;

    if (!area) {
        mvprintw(LINES-2, 0, "Err. Area is NULL.\n");
        refresh();
        return STATUS_ERR_NULL_PTR;
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
        return STATUS_ERR_FILE_OPEN;
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
    return STATUS_OK;
}
