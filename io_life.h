#ifndef IO_LIFE_H
#define IO_LIFE_H

int readLifeArea(options_t*);

int saveLifeArea(const area_t*);

int enterInt(int*);

int enterSize(size_t* n);

void printOptions(options_t*);

void defaultChoose(void);

void printActions(void);

int printArea(const area_t*, int, int);

#endif
