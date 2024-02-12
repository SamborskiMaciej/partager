#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

void help()
{
    char * help = "Usage: partager [OPTIONS] ip\n"
                  "Straight forward communication over the internet\n"
                  "\nOptions:\n"
                  "";
    printf("%s\n", help);
    exit(0);
}

partresult parseArgs(int argc, char ** argv, State * state)
{
    for (int i = 1; i < argc; ++i)
    {
        if (*argv[i] == '-')
        {
            char * value = argv[i + 1];

            if (!strcmp("-p", argv[i]) ||
                !strcmp("--port", argv[i])
               )
            {
                state->conf.port = atoi(value);
                goto skip;
            }

            continue;

            skip:
                ++i;
        } else
        {
            if (!strcmp(argv[i], "server"))
            {
                state->conf.mode = PARTSERVER;
            }
            else if (!strcmp(argv[i], "client"))
            {
                state->conf.mode = PARTCLIENT;
            }
            else if (i == argc - 1) //TODO: replace with proper ip address validation and add domain support
            {
                strncpy(state->conf.ip, argv[i], IPADDRLEN);
            }
            else
                return -1;
        }
    }

    return 0;
}
