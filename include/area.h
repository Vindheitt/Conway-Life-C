#ifndef AREA_H
#define AREA_H

status_t destroyArea(area_t**);

status_t createArea(area_t**, size_t, size_t);

void setCell(area_t*, int, int, char);

char getCell(const area_t*, int, int);

#endif
