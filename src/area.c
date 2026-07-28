#include "life_system.h"
#include "area.h"

status_t destroyArea(area_t **area) {
    if (!area || !(*area))
        return STATUS_ERR_NULL_PTR;
    free((*area)->matrix);
    free(*area);
    *area = NULL;
    return STATUS_OK;
}
status_t createArea(area_t** area, size_t rows, size_t cols) {
    if (!area)
        return STATUS_ERR_NULL_PTR;
    *area = malloc(sizeof(area_t));
    if(!(*area))
        return STATUS_ERR_MEMORY;
    (*area)->rows = rows;
    (*area)->cols = cols;
    (*area)->matrix = calloc(rows * cols, sizeof(char));
    if(!(*area)->matrix){
        free(*area);
        return STATUS_ERR_MEMORY;
    }
    return STATUS_OK;
}
void setCell(area_t *area, int y, int x, char val) {
    area->matrix[y * area->cols + x] = val;
}
char getCell(const area_t *area, int y, int x) {
    return area->matrix[y * area->cols + x];
}
