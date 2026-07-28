#include "life_system.h"
#include "file_io.h"

#include "area.h"
#include "utils.h"

status_t readLifeArea(area_t** area, options_t* config, char* filename){
    size_t rows, cols;
    size_t y, x;

    int c;

    FILE *file;

    area_t* newArea;

    if(!area || !config)
        return STATUS_ERR_NULL_PTR;

    file = fopen(filename, "r");
    if (!file)
        return STATUS_ERR_FILE_OPEN;

    if (fscanf(file, "%zu %zu", &rows, &cols) != 2) {
        fclose(file);
        return STATUS_ERR_FILE_FORMAT;
    }

    config->rows = rows;
    config->cols = cols;

    createArea(&newArea, rows, cols);
    if (!newArea) {
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

    fclose(file);
    *area = newArea;

    return STATUS_OK;
}
status_t saveLifeArea(const area_t* area, char* filename) {
    size_t y,x;
    size_t rows;
    size_t cols;

    FILE *file;

    if (!area || !filename)
        return STATUS_ERR_NULL_PTR;

    file = fopen(filename, "w");
    if (!file)
        return STATUS_ERR_FILE_OPEN;

    rows = area->rows;
    cols = area->cols;

    fprintf(file, "%zu %zu\n", rows, cols);

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++)
            fputc(getCell(area, y,x) ? '1' : '0', file);
        fputc('\n', file);
    }

    fclose(file);
    return STATUS_OK;
}
