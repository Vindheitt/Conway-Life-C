#ifndef UI_H
#define UI_H

status_t uiLogic(char*, char*[], int, int*);

status_t mainMenu(area_t*, options_t*);

status_t printArea(const area_t*, int, int);

status_t moveAndChange(area_t*);

status_t printSimulatuon(const area_t*, int, int, int);

status_t saveLifeAreaUI(const area_t*);

status_t readLifeAreaUI(area_t**, options_t*);

status_t optionsMenu(options_t*);

status_t actionOptions(options_t*, int);

status_t chooseSize(options_t*);

void printOptions(options_t*);

#endif
