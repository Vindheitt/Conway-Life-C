#include "all_src_files.h"

int changeOptions(options_t* config){
    int userChoose;
    if(!config)
        return -1;
    clear();
    do{
        printw("Choose options\n");
        do{
            printOptions(config);
            printw("Enter your choose: ");
            refresh();
        }while(enterInt(&userChoose));

        actionOptions(config, userChoose);
    }while(userChoose);
    clear();
    printw("What can I do for you?\n");
    return 0;
}
int actionOptions(options_t* config, int userChoose){
    int newValue;
    if(!config)
        return -1;
    switch (userChoose) {
        case 1:
            do{
                clear();
                printw("Choose type of rule:\n");
                printw("\t0)Outside = toroidal\n");
                printw("\t1)Outside = dead\n");
                printw("Enter your choose: ");
            }while(enterInt(&newValue) || (newValue != 1 && newValue !=0));
            config->rule = newValue;
            break;
        case 2:
            do{
                clear();
                printw("Enter new value (ms, >=100): ");
            }while(enterInt(&newValue) || (newValue < 100));
            config->waitTime = newValue;

            break;
        case 3:
            chooseSize(config);
            break;
        case 0:

            break;
        default:
            defaultChoose();
            break;
    }
    clear();
    return 0;
}
int chooseSize(options_t* config){
    size_t rows = 0;
    size_t cols = 0;

    if(!config)
        return -1;

    clear();
    while(rows < 1 || (int)rows > LINES - 2){
        do{
            printw("Please enter quantity of rows (1..%d): ", LINES - 2);
            refresh();
        }while(enterSize(&rows));
        if(rows < 1 || (int)rows > LINES - 2){
            clear();
            printw("Incorrect value. Please try again.\n");
        }
    }
    clear();
    while(cols < 1 || (int)cols > (COLS/2) - 1){

        do{
            printw("Please enter quantity of columns (1..%d): ", (COLS/2) - 1);
            refresh();
        }while(enterSize(&cols));
        if(cols < 1 || (int)cols > (COLS/2) - 1){
            clear();
            printw("Incorrect value. Please try again.\n");
        }
    }
    config->rows = rows;
    config->cols = cols;

    return 0;
}
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
