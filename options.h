#ifndef OPTIONS_H
#define OPTIONS_H

int waitMs(int);

int changeOptions(options_t*);

int actionOptions(options_t*, int);

int chooseSize(options_t*);

options_t* makeOptions();

#endif
