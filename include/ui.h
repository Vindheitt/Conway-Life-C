#ifndef UI_H
#define UI_H

status_t menuDrawUI(char*, char*[], int, int*);

status_t mainMenu(area_t**, options_t*);

status_t printArea(const area_t*, int, int);

status_t moveAndChange(area_t**);

status_t printSimulation(const area_t*, int, int, int);

status_t saveLifeAreaUI(const area_t*);

status_t readLifeAreaUI(area_t**, options_t*);

status_t optionsMenu(options_t*);

status_t changeOptions(options_t*, int);

status_t changeRulesUI(options_t*);

status_t changeWaitTimeUI(options_t*);

status_t changeSizeUI(options_t*);

status_t printError(status_t);

const char* statusToString(status_t);

#endif
