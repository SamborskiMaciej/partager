#include <curses.h>
#include <errno.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>

#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <wchar.h>

#include "term.h"

#define DA_IMPLEMENTATION
#include "da.h"

#define bufferLen 256
#define PORT 12123

enum {
    SENDER       = 0,
    RECEIVER     = 1,
    THREAD_COUNT = 2,
};

typedef int fd_t;

void * sender(void * arg)
{
    fd_t * connFd = arg;

    return NULL;
}

void * receiver(void * arg)
{
    char buffer[bufferLen] = {0};

    fd_t * connFd = arg;

    while (1)
    {
        read(*connFd, buffer, bufferLen);
        printf("\033[A");
        printf("\n");
        printf("%s", buffer);
        if (strcmp(buffer, "e\n") == 0) break;

        bzero(buffer, bufferLen);
    }

    return NULL;
}

void serve()
{
    int sockFd, connFd;
    struct sockaddr_in server, client;
    bzero(&server, sizeof(server));
    bzero(&client, sizeof(client));

    sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockFd < 0)
    {
        fprintf(stderr, "ERROR:%d:%s\n", errno, strerror(errno));
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
    server.sin_port = htons(PORT);

    if (bind(sockFd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        fprintf(stderr, "ERROR:%d:%s\n", errno, strerror(errno));
        exit(1);
    }

    if (listen(sockFd, 0) < 0)
    {
        fprintf(stderr, "ERROR:%d:%s\n", errno, strerror(errno));
        exit(1);
    }

    socklen_t len = sizeof(client);
    connFd = accept(sockFd, (struct sockaddr *) &client, &len);
    if (connFd < 0)
    {
        fprintf(stderr, "ERROR:%d:%s\n", errno, strerror(errno));
        exit(1);
    }

    pthread_t threads[THREAD_COUNT];
    int threadError = 0;

    threadError = pthread_create(&threads[SENDER], NULL, sender, &connFd);
    if (threadError)
    {
        fprintf(stderr, "ERROR:Couldn't create SENDER Thread\n");
        exit(1);
    }

    threadError = pthread_create(&threads[RECEIVER], NULL, receiver, &connFd);
    if (threadError)
    {
        fprintf(stderr, "ERROR:Couldn't create RECEIVER Thread\n");
        exit(1);
    }

    for (size_t i = 0; i < THREAD_COUNT; ++i)
        pthread_join(threads[i], NULL);

    close(sockFd);
}

typedef struct State {
    Term term;
    dynarr(char *) messages;
} State;

void draw(State * state)
{
    int x, y;
    getmaxyx(stdscr, y, x);
    if (state->term.termX != x ||
        state->term.termY != y)
    {
        recreateWindows(&state->term);
    }

    wmove(state->term.chat, 1, 2);
    wmove(state->term.input, state->term.inputY, state->term.inputX);

    for (size_t i = 0; i < daSize(state->messages); ++i)
    {
        wprintw(state->term.chat, "%s", state->messages[i]);
        wmove(state->term.chat, 2 + i, 2);
    }

    wrefresh(state->term.chat);
    wrefresh(state->term.extra);
    wrefresh(state->term.input);
}

void loop(State * state)
{
    initNcurses();

    state->term = createTerm();
    draw(state);

    dynarr(char) buffer = daCreate(char, 128);
    heapstr msg = NULL;

    int key = 0;
    while ((key = getch()) != 27 /* ESC */)
    {
        switch (key)
        {
        case 10: /* ENTER */
            msg = daToCStr(buffer);
            daPush(state->messages, msg);
            daBzero(buffer);
            state->term.inputX = 1;

            for (size_t i = 1; i < getmaxx(state->term.input) - 1; ++i)
                mvwprintw(state->term.input, state->term.inputY, i, " ");

            break;

        case KEY_LEFT:
            if (state->term.inputX > 1)
                state->term.inputX -= 1;
            break;

        case KEY_RIGHT:
            if (state->term.inputX < getmaxx(state->term.input) - 2)
                state->term.inputX += 1;
            break;

        case KEY_UP:
        case KEY_DOWN:
            break;

        case KEY_BACKSPACE:
            if (state->term.inputX > 1)
            {
                state->term.inputX -= 1;
                wmove(state->term.input, state->term.inputY, state->term.inputX);
                wprintw(state->term.input, " ");

                if (daSize(buffer))
                    daPopDiscard(buffer);
            }
            break;

        case KEY_F(1):
            recreateWindows(&state->term);
            break;

        case 410: /* F11 */
            break;

        default:
            if (state->term.inputX < getmaxx(state->term.input) - 2)
            {
                wprintw(state->term.input, "%c", key);
                daPush(buffer, key);
                state->term.inputX += 1;
            }
        }

        draw(state);
    }

    deinitNcurses();
    daFree(buffer);
}

int main(void)
{
    State state = {
        .term = { },
        .messages = daCreate(char *, 64),
    };

    loop(&state);

    for (size_t i = 0; i < daSize(state.messages); ++i)
    {
        free(state.messages[i]);
    }

    daFree(state.messages);

    return 0;
}
