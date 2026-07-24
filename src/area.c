#include "all_src_files.h"

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
