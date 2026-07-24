#ifndef AREA_H
#define AREA_H

void destroyData(area_t**);

void endOfProgramm(void);

void setCell(area_t*, int, int, char);

char getCell(const area_t*, int, int);

area_t* createArea(size_t, size_t);

#endif
