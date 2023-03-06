#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>

#include "plstatus.h"
#include "config.h"

#define until_components_end(index) components[index].command != NULL

// Print the string stored in status on the display status bar
#define show_status(display, status) XStoreName(display, DefaultRootWindow(display), status); XFlush(display)

extern Component components[];

Display *display;

char status[MAX_LEN + 1];
sem_t status_mutex;

void termination_handler() {
    for(int i = 0; until_components_end(i); i++) {
        if(components[i].thread_joined) continue;

        pthread_kill(components[i].thread, SIGUSR1);
        pthread_join(components[i].thread, NULL);
    }

    show_status(display, NULL);
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
        show_status(display, status);

        usleep(UPDATE_PERIOD * 1000);
    }
}

void failure(char *err) {
    fprintf(stderr, "%s", err);
    exit(EXIT_FAILURE);
}

void init() {
    sem_init(&status_mutex, 0, 1);

    for(int i = 0; until_components_end(i); i++) {
        components[i].current_result[0] = '\0';

        sem_init(&components[i].ran_once, 0, 0);
        pthread_create(&(components[i].thread), NULL, (void *) component_thread, &components[i]);

        sem_wait(&components[i].ran_once);
        if(components[i].period == __UINT32_MAX__)
            pthread_join(components[i].thread, NULL);
    }

}

void get_status() {
    sem_wait(&status_mutex);

    status[0] = '\0';
    for(int i = 0; until_components_end(i); i++) {
        if(strncmp(components[i].current_result, "", strlen(components[i].current_result))) {
            strncat(status, components[i].current_result, MAX_LEN - strlen(status));
            strncat(status, components[i].sep, MAX_LEN - strlen(status));
        }
    }

    sem_post(&status_mutex);
}

void component_stop() {
    pthread_exit(EXIT_SUCCESS);
}

void run_component(Component *component) {
    char buf[MAX_RESULT_LEN];
    get_component_output(buf, component);
    wait(NULL);

    // Remove newline
    buf[strcspn(buf, "\n")] = 0;

    sem_wait(&status_mutex);
    strncpy(component->current_result, buf, MAX_RESULT_LEN);
    sem_post(&status_mutex);
}

void component_thread(void *component_ptr) {
    Component *component = (Component *) component_ptr;
    component->period = component->period < __UINT32_MAX__ / 1000 ? \
                        component->period * 1000 : __UINT32_MAX__;

    struct sigaction action = {0};
    action.sa_handler = component_stop;

    sigaction(SIGUSR1, &action, NULL);

    run_component(component);
    sem_post(&component->ran_once);

    if(component->period == __UINT32_MAX__) {
        component->thread_joined = True;
        component_stop();
    }

    while(True) {
        run_component(component);

        usleep(component->period);
    }
}

void get_component_output(char *dest, Component *component) {
    int piped[2];
    if(pipe(piped) == -1)
        failure("Couldn't create pipe\n");

    if(fork() == 0) {
        close(piped[0]);
        dup2(piped[1], 1);
        dup2(piped[1], 2);
        close(piped[1]);

        execvp(component->command, component->arguments);
        failure("Exec failed \n");
    }
    else {
        dest[0] = '\0';

        // Bytes read into buffer each time
        int c = 0;

        char buf[1024];
        buf[0] = '\0';

        close(piped[1]);

        while((c = read(piped[0], buf, sizeof(buf) - 1))) {
            // Sets the byte after the last one read to '\0', terminating the string
            buf[c] = '\0';

            strncat(dest, buf, MAX_RESULT_LEN - strlen(dest));

            if(strlen(dest) + strlen(buf) >= (size_t) MAX_RESULT_LEN)
                break;
        }
        close(piped[0]);
    }
}
