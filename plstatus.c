#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <X11/Xlib.h>

#include "plstatus.h"
#include "config.h"

extern Component components[];

Display *display;

char status[MAX_LEN];
sem_t status_mutex;

void termination_handler() {
    for(int i = 0; components[i].command != NULL; i++) {
        pthread_kill(components[i].thread, SIGUSR1);
        pthread_join(components[i].thread, NULL);
    }

	if(XCloseDisplay(display) < 0)
		failure("Could not close display\n");

    exit(EXIT_SUCCESS);
}

int main() {
	display = XOpenDisplay(NULL);
	if(display == NULL)
		failure("Could not open display\n");

    init();

    struct sigaction action = {0};
    action.sa_handler = termination_handler;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    while(True) {
        get_status();

        XStoreName(display, DefaultRootWindow(display), status);
        XFlush(display);

        usleep(UPDATE_PERIOD * 1000);
    }
}

void failure(char *err) {
    fprintf(stderr, "%s", err);
    exit(EXIT_FAILURE);
}

void init() {
    sem_init(&status_mutex, 0, 1);

    for(int i = 0; components[i].command != NULL; i++) {
        components[i].current_result[0] = '\0';
        pthread_create(&(components[i].thread), NULL, (void *) component_thread, &components[i]);
    }
}

void get_status() {
    sem_wait(&status_mutex);
    status[0] = '\0';
    for(int i = 0; components[i].command != NULL; i++) {
        strncat(status, components[i].current_result, MAX_LEN - strlen(status));
    }
    sem_post(&status_mutex);
}

void component_stop() {
    pthread_exit(EXIT_SUCCESS);
}

void component_thread(void *component_ptr) {
    Component *component = (Component *) component_ptr;
    unsigned int period = component->period < __UINT32_MAX__ / 1e3 ? \
                          component->period * 1e3 : __UINT32_MAX__;
    
    struct sigaction action = {0};
    action.sa_handler = component_stop;

    sigaction(SIGUSR1, &action, NULL);

    while(True) {
        char buf[MAX_RESULT_LEN];
        get_component_output(buf, component);
        wait(NULL);

        // Remove newline
        buf[strcspn(buf, "\n")] = 0;

        sem_wait(&status_mutex);
        strncpy(component->current_result, buf, MAX_RESULT_LEN);
        sem_post(&status_mutex);

        usleep(period);
    }
}

void get_component_output(char *temp_dest, Component *component) {
    int piped[2];
    if(pipe(piped) == -1) {
        exit(EXIT_FAILURE);
    }

    if(fork() == 0) {
        close(piped[0]);
        dup2(piped[1], 1);
        dup2(piped[1], 2);
        close(piped[1]);

        execvp(component->command, component->arguments);
        exit(EXIT_FAILURE);
    }
    else {
        temp_dest[0] = '\0';

        int c = 0; // Bytes read into buffer each time
        char buf[1024];
        buf[0] = '\0';

        close(piped[1]);

        while((c = read(piped[0], buf, sizeof(buf) - 1))) {
            // Sets the byte after the last one read to '\0', terminating the string
            buf[c] = '\0';

            if(strlen(temp_dest) + strlen(buf) >= (size_t) (MAX_RESULT_LEN - 1))
                break;

            strcat(temp_dest, buf);
        }
        close(piped[0]);
    }
}
