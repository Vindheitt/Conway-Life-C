#include "life_system.h"
#include "utils.h"

status_t enterInt(int *n) {
    char buf[32];
    int result;

    if (!n)
        return STATUS_ERR_NULL_PTR;

    echo();
    curs_set(1);

    getnstr(buf, sizeof(buf) - 1);
    result = sscanf(buf, "%d", n);

    noecho();
    curs_set(0);
    clrtoeol();

    if(result != 1)
        return STATUS_ERR_INVALID_INPUT;
    return STATUS_OK;
}
status_t enterSize(size_t *n) {
    char buf[32];
    int result;

    if (!n)
        return STATUS_ERR_NULL_PTR;

    echo();
    curs_set(1);

    getnstr(buf, sizeof(buf) - 1);
    result = sscanf(buf, "%zu", n);

    noecho();
    curs_set(0);
    clrtoeol();

    if(result != 1)
        return STATUS_ERR_INVALID_INPUT;
    return STATUS_OK;
}
int waitMs(int ms) {
    if (ms <= 0)
        ms = DEFAULT_WAIT;
    #ifdef WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
    return 0;
}
