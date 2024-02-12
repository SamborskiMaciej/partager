#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#include "term.h"

#include "da.h"

#define IPADDRLEN 15

typedef int fd_t;

typedef enum OperationMode {
    PARTMODEINVALID = 0,
    PARTSERVER      = 1,
    PARTCLIENT      = 2,
    PARTMODECOUNT   = 3,
} OperationMode;

typedef struct Config {
    char ip[IPADDRLEN];
    uint16_t port;
    OperationMode mode;
} Config;

typedef struct State {
    Term term;
    Config conf;
    dynarr(char *) messages;
} State;

State createDefaultState();

typedef int partresult; // TODO: Implement proper error reporting system

#endif // COMMON_H_
