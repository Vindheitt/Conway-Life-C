#ifndef LIFE_SYSTEM_H
#define LIFE_SYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#include <ncurses.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1

#define OUTSIDE_DEAD 0
#define OUTSIDE_TOROIDAL 1

#define DEFAULT_HEIGHT 20
#define DEFAULT_WIDTH  40

#define DEFAULT_WAIT 100
#define SAVE_WAIT 2000

#define BUFFER_SIZE 32

#define ENTER 10

#define swap(t, x, y) do{t temp = x; x = y; y = temp;} while(0)

typedef enum{
    STATUS_OK = 0,
    STATUS_EXIT,
    STATUS_ERR_NULL_PTR,
    STATUS_ERR_MEMORY,
    STATUS_ERR_FILE_OPEN,
    STATUS_ERR_FILE_FORMAT,
    STATUS_ERR_INVALID_INPUT,
    STATUS_ERR_EMPTY_STR
}status_t;

typedef struct Area{
    size_t cols;
    size_t rows;

    char *matrix;
}area_t;

typedef struct Options{
    int rule;
    //int neighbors;
    //int waitTime;
    size_t waitTime;
    size_t cols;
    size_t rows;
}options_t;

#endif
