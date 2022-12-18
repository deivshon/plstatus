#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>

#include "../hs/plstatus.h"
#include "../hs/config.h"

#define until_components_end(index) components[index].command != NULL

// Print the string stored in status on the display status bar
#define show_status(display, status) XStoreName(display, DefaultRootWindow(display), status); XFlush(display)

// Sleep the sleep_time_ms value and then update the status bar with the current components' results
#define sleep_update(sleep_time_ms)     \
        usleep(sleep_time_ms * 1000);   \
        get_status();                   \
        show_status(display, status)

extern Component components[];

Display *display;

char status[MAX_LEN + 1];
sem_t status_mutex;

void termination_handler() {
    for(int i = 0; until_components_end(i); i++) {
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

    // Avoid waiting the whole update period before printing the first status
    sleep_update(10);

    while(True) {
        sleep_update(UPDATE_PERIOD);
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
        pthread_create(&(components[i].thread), NULL, (void *) component_thread, &components[i]);
    }
}

void get_status() {
    sem_wait(&status_mutex);

    status[0] = '\0';
    for(int i = 0; until_components_end(i); i++) {
        strncat(status, components[i].current_result, MAX_LEN - strlen(status));
    }

    sem_post(&status_mutex);
}

void component_stop() {
    pthread_exit(EXIT_SUCCESS);
}

void component_thread(void *component_ptr) {
    Component *component = (Component *) component_ptr;
    unsigned int period = component->period < __UINT32_MAX__ / 1000 ? \
                          component->period * 1000 : __UINT32_MAX__;

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
