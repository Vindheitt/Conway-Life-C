#ifndef UI_H
#define UI_H

int changeOptions(options_t*);

int actionOptions(options_t*, int);

int chooseSize(options_t*);

int action(int, options_t*);

void printOptions(options_t*);

void defaultChoose(void);

void printActions(void);

int printArea(const area_t*, int, int);

int moveAndChange(area_t*);

#endif
