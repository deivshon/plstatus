#include <stddef.h>
#include "plstatus.h"

#define separator(str) {"printf", {"printf", str, NULL}, __UINT32_MAX__}

#define MAX_LEN 2048
#define UPDATE_PERIOD 100

/* 
    Component syntax: {command, {command, arg1, arg2 ... NULL}, period}
    Note: components[] must be NULL terminated

    Example:
    Component components[] = {
        {"ls", {"ls", "-l", "-a", NULL}, 1},
        {"ls", {"ls", NULL}, 1},
        NULL
    };
*/
Component components[] = {
    {"date", {"date", "+%S", NULL}, 100},
    separator(" | "),
    {"date", {"date", "+%S", NULL}, 500},
    separator(" | "),
    {"date", {"date", "+%S", NULL}, 2500},
    NULL
};
