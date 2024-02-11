#include "term.h"
#include <curses.h>
#include <stdlib.h>

Term createTerm()
{
    int x, y;
    getmaxyx(stdscr, y, x);

    WINDOW * chatWin = newwin(y - 7, x - 48, 1, 1);
    WINDOW * extraWin = newwin(y - 1, 46, 1, x - 47);
    WINDOW * inputWin = newwin(6, x - 48, y - 6, 1);

    box(chatWin, 0, 0);
    box(inputWin, 0, 0);
    box(extraWin, 0, 0);

    wrefresh(chatWin);
    wrefresh(inputWin);
    wrefresh(extraWin);

    return (Term) {
        .termX = x,
        .termY = y,
        .chat = chatWin,
        .extra = extraWin,
        .input = inputWin,
        .inputY = getmaxy(inputWin) - 2,
        .inputX = 1,
    };
}

void recreateWindows(Term * term)
{
    int x, y;
    getmaxyx(stdscr, y, x);

    term->termX = x;
    term->termY = y;

    wborder(term->chat, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wborder(term->input, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wborder(term->extra, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    wrefresh(term->chat);
    wrefresh(term->input);
    wrefresh(term->extra);

    delwin(term->chat);
    delwin(term->input);
    delwin(term->extra);

    term->chat = newwin(y - 7, x - 48, 1, 1);
    term->extra = newwin(y - 1, 46, 1, x - 47);
    term->input = newwin(6, x - 48, y - 6, 1);

    box(term->chat,  0, 0);
    box(term->input, 0, 0);
    box(term->extra, 0, 0);
}

void initNcurses()
{
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
}

void deinitNcurses()
{
    endwin();
    system("reset");
}
