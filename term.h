#ifndef TERM_H_
#define TERM_H_

#include <ncurses.h>

typedef struct Term {
    int inputX, inputY;
    WINDOW * chat, * input, * extra;
    size_t termX, termY;
} Term;

Term createTerm();
void recreateWindows(Term * term);
void initNcurses();
void deinitNcurses();

#endif // TERM_H_
