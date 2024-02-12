#include "common.h"

State createDefaultState()
{
    return (State) {
        .term = {0},
        .conf = {
            "",
            0,
            PARTMODEINVALID,
        },
        .messages = daCreate(char *, 64),
    };
}
