#ifndef PLSTATUS
#define PLSTATUS

#include <pthread.h>

#define MAX_RESULT_LEN 256
#define MAX_TOKENS 128

void init();
void failure(char *err);

typedef struct component {
    char *command;
    char *arguments[MAX_TOKENS];
    unsigned int period;
    char current_result[MAX_RESULT_LEN];
    pthread_t thread;
} Component;

void get_status();
void component_thread(void *component);
void get_component_output(char *temp_dest, Component *component);

#endif
