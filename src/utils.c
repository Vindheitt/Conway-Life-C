int enterInt(int *n) {
    char buf[32];

    if (!n)
        return -1;

    echo();
    curs_set(1);
    while (TRUE) {
        getnstr(buf, sizeof(buf)-1);
        if (sscanf(buf, "%d", n) == 1)
            break;
        clear();
        printw("Something went wrong with enter int..\n");
        refresh();
        clrtoeol();
        refresh();
        noecho();
        curs_set(0);
        return 1;
    }
    curs_set(0);
    noecho();
    return 0;
}
int enterSize(size_t *n) {
    char buf[32];

    if (!n)
        return -1;

    echo();
    curs_set(1);
    while (TRUE) {
        getnstr(buf, sizeof(buf)-1);
        if (sscanf(buf, "%zu", n) == 1)
            break;
        clear();
        printw("Something went wrong with enter int..\n");
        refresh();
        clrtoeol();
        refresh();
        noecho();
        curs_set(0);
        return 1;
    }
    curs_set(0);
    noecho();
    return 0;
}
int waitMs(int ms) {
    if (ms <= 0) return -1;
    #ifdef WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
    return 0;
}
