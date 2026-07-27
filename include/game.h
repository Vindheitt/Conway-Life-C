#ifndef GAME_H
#define GAME_H

status_t startGame(area_t**,options_t*);

status_t startSimulation(area_t*,options_t*);

status_t countOfAlive(const area_t*, int*);

status_t nextGeneration(area_t*, area_t*, options_t*);

status_t checkCell(const area_t*, int, int, int*, options_t*);

#endif
