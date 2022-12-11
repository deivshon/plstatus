#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <X11/Xlib.h>

#include "plstatus.h"
#include "config.h"

#define loop while(1)

Display *display;

char status[MAX_LEN];
sem_t status_mutex;

extern Component components[];

int main() {
	display = XOpenDisplay(NULL);
	if(display == NULL)
		failure("Could not open display\n");

    init();

    loop {
        get_status();

        XStoreName(display, DefaultRootWindow(display), status);
        XFlush(display);

        usleep(UPDATE_PERIOD * 1000);
    }

	if(XCloseDisplay(display) < 0)
		failure("Could not close display\n");
}

void failure(char *err) {
    fprintf(stderr, "%s", err);
    exit(EXIT_FAILURE);
}

void init() {
    for(int i = 0; components[i].command != NULL; i++) {
        components[i].current_result[0] = '\0';
        pthread_create(&(components[i].thread), NULL, (void *) component_thread, &components[i]);
    }
    sem_init(&status_mutex, 0, 1);
}

void get_status() {
    sem_wait(&status_mutex);
    status[0] = '\0';
    for(int i = 0; components[i].command != NULL; i++) {
        strncat(status, components[i].current_result, MAX_LEN - strlen(status));
    }
    sem_post(&status_mutex);
}

void component_thread(void *component_ptr) {
    Component *component = (Component *) component_ptr;

    loop {
        char buf[MAX_RESULT_LEN];
        get_component_output(buf, component);

        // Strip newline
        buf[strcspn(buf, "\n")] = 0;

        sem_wait(&status_mutex);
        strncpy(component->current_result, buf, MAX_RESULT_LEN);
        sem_post(&status_mutex);

        usleep(component->period * 1000);
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

            if(strlen(temp_dest) + strlen(buf) >= (size_t) MAX_RESULT_LEN)
                break;

            strcat(temp_dest, buf);
        }
        close(piped[0]);
    }
}
