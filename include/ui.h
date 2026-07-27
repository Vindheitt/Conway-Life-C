#ifndef UI_H
#define UI_H

status_t uiLogic(char*, char*[], int, int*);

status_t mainMenu(area_t*, options_t*);

status_t printArea(const area_t*, int, int);

status_t moveAndChange(area_t*);

int changeOptions(options_t*);

int actionOptions(options_t*, int);

int chooseSize(options_t*);

void printOptions(options_t*);

#endif
