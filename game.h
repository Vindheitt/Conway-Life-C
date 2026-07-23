#ifndef GAME_H
#define GAME_H

int startGame(area_t*,options_t*);

int startSimulation(area_t*,options_t*);

int countOfAlive(const area_t*);

int nextGeneration(area_t*, area_t*, options_t*);

int checkCell(const area_t*, int, int, options_t*);

#endif
