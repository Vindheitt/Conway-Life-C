#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// #ifdef _WIN32
//     #include <curses.h>
//     #include <windows.h>
// #else
//     #include <ncurses.h>
//     #include <unistd.h>
// #endif

#include <ncurses.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define OUTSIDE_DEAD 1
#define OUTSIDE_TOROIDAL 0

#define DEFAULT_HEIGHT 20
#define DEFAULT_WIDTH  40

#define DEFAULT_WAIT 100
#define SAVE_WAIT 2000

//#define swap(t, x, y) do{t temp = x; x = y; y = temp;} while(0)
#define swap(t, x, y) {t temp = x; x = y; y = temp;}

typedef struct Area{
    size_t cols;
    size_t rows;

    char *matrix;
}area_t;

typedef struct Options{
    int rule;
    /*int neighbors;*/
    int waitTime;

    size_t cols;
    size_t rows;
}options_t;
