#include "all_src_files.h"

options_t* makeOptions(){
    options_t* config = malloc(sizeof(options_t));
    if (!config)
        return NULL;
    config->rule = OUTSIDE_DEAD;
    config->waitTime = DEFAULT_WAIT;

    config->cols = DEFAULT_WIDTH;
    config->rows = DEFAULT_HEIGHT;
    return config;
}
