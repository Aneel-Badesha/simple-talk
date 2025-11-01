#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "keyb_in.h"
#include "udp_in.h"
#include "udp_out.h"
#include "screen_out.h"
#include <pthread.h>

#define PORT 6060

static pthread_t threadPID;
static List* received;
static char* msg;
static pthread_mutex_t trim_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t perms = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;

void* screen_out_thread(void* arg) {
    while (true) {

        // write lock to make sure it doesn't print at the wrong time considering it shares the list data among all other threads
        pthread_mutex_lock(&write_lock);
        {
            pthread_cond_wait(&perms, &write_lock);
        }
        pthread_mutex_unlock(&write_lock);

        // trim lock to make sure it doesnt trim the list when its being accessed elsewhere
        pthread_mutex_lock(&trim_lock);
        {
            msg = List_trim(received);
        }
        pthread_mutex_unlock(&trim_lock);

        //check for null msg
        if (msg == NULL) {
            printf("empty message\n");
            continue;
        }

        // write msg to stdout, (tried using printf with stdout but had errors)
        int ret = write(1, msg, strlen(msg));

        if (ret == -1) {
            perror("error writing\n");
        }

        // check for termination message, cancel all threads
        if (strcmp(msg, "!\n") == 0) {
            udp_in_cancel();
            udp_out_cancel();
            screen_out_cancel();
            keyb_in_cancel();
        }
    }
    return NULL;
}

// initialize list and create thread
void screen_out_init(List* list) {
    received = list;
    int check = pthread_create(&threadPID, NULL, screen_out_thread, NULL);
    if (check != 0) {
        perror("screen_out thread error!");
        exit(1);
    }
}

// gives the signal to start printing out to screen
void screen_out_start() {

    pthread_mutex_lock(&write_lock);
    {
        pthread_cond_signal(&perms);
    }
    pthread_mutex_unlock(&write_lock);
}

// cancel current thread
void screen_out_cancel(void) {
    pthread_cancel(threadPID);
}

void screen_out_shutdown(void) {
    pthread_join(threadPID, NULL);
}