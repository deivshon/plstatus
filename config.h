#include <stddef.h>
#include "plstatus.h"

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
    {"echo", {"echo", " | ", NULL}, 10000},
    {"date", {"date", "+%S", NULL}, 500},
    {"echo", {"echo", " | ", NULL}, 10000},
    {"date", {"date", "+%S", NULL}, 2500},
    NULL
};
