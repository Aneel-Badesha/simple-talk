#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "keyb_in.h"
#include "udp_out.h"
#include "udp_in.h"
#include <pthread.h>

#define PORT 6060
#define MESSAGE_SIZE 256

static pthread_t threadPID;
static List* send_list;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

char* input;

void *keyb_in_thread(void *arg) {

    // char array with max size 256
    
    while (true) {
        // reads from stdin into raw_input
        char input[MESSAGE_SIZE];
        fgets(input, sizeof(input), stdin);
        
        // temp for error checking adding to the list
        char* pointed_input = strdup(input);
        int check;

        //append message to send list, put inside a lock so that only one thread at a time is accessing the shared list data
        pthread_mutex_lock(&lock);
        {
            check = List_prepend(send_list, pointed_input);
        }
        pthread_mutex_unlock(&lock);

        // error check
        if (check == -1) {
            // if temp comes back as -1 (fail), print error and free memory from pointed input
            printf("Error during keyboard input!\n");
            free(pointed_input);
        }
        /*SEND MESSAGE HERE*/
        udp_out_start();
    }

    return NULL;
}

void keyb_in_init(List* list) {
    send_list = list;

    // create thread (check will store int corresponding to success or fail)
    int check = pthread_create(&threadPID, NULL, keyb_in_thread, NULL);

    // if fail, do message and exit
    if (check != 0) {
        perror("keyb_in thread error!");
        exit(1);
    }
}

// sends cancellation request to the keyb_in thread
void keyb_in_cancel(void) {
    pthread_cancel(threadPID);
}


void keyb_in_shutdown(void) {
    pthread_join(threadPID, NULL);
}