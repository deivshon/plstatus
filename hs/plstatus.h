#ifndef PLSTATUS
#define PLSTATUS

#include <pthread.h>
#include <semaphore.h>

#define MAX_RESULT_LEN 256
#define MAX_TOKENS 128

void init();
void failure(char *err);

typedef struct component {
    char *command;
    char *arguments[MAX_TOKENS];

    unsigned int period;

    char current_result[MAX_RESULT_LEN + 1];

    pthread_t thread;
    int thread_joined;

    sem_t ran_once;
} Component;

void get_status();
void component_thread(void *component);
void get_component_output(char *dest, Component *component);

#endif
