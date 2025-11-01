#include "udp_in.h"
#include "list.h"

#define MESSAGE_SIZE 256

// thread static variables
int PORT;
static pthread_t threadPID;
static List* received;
static int send_socket;
static pthread_mutex_t lock;

void *udp_in_thread(void *arg) {

    // create the server address structure
    struct sockaddr_in sock_in;
    memset(&sock_in, 0, sizeof(sock_in));

    // address fam to IPV4, listen on all interfaces, and sets the port number
    sock_in.sin_family = AF_INET;
    sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_in.sin_port = htons(PORT);

    // creating UDP socket
    int send_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (send_socket == -1) {
        perror("Error creating socket\n");
        exit(1);
    }

    // Binds the socket to the specified port
    int check = bind(send_socket, (struct sockaddr*)&sock_in, sizeof(sock_in));

    if (check == -1) {
        perror("Error binding\n");
        exit(1);
    }

    // loop to continuously recieve messages
    while (true) {

        // input buffer
        char input[MESSAGE_SIZE];

        // structure to store senders addr info
        struct sockaddr_in sock_rem;
        unsigned int len = sizeof(sock_rem);

        // recieve message from socket
        int mem_rec = recvfrom(send_socket, input, MESSAGE_SIZE, 0, (struct sockaddr*)&sock_rem, &len);

        // determines end of message and null terminates it
        int end;
        if (mem_rec < MESSAGE_SIZE) {
            end = mem_rec;
        }
        else {
            end = MESSAGE_SIZE -1;
        }
        input[end] = 0;

        // mutex for prepending message
        int check1;
        pthread_mutex_lock(&lock);
        {
            check1 = List_prepend(received, input);
        }
        pthread_mutex_unlock(&lock);

        if (check1 == -1) {
            printf("Error prepending input to list\n");
        }

        // signals the screen out wait condition to start printing
        screen_out_start();
    }
    return NULL;
}


void udp_in_init(List* list, int localport) {

    received = list;
    PORT = localport;

    //create thread (check will store int corresponding to success or fail)
    int check = pthread_create(&threadPID, NULL, udp_in_thread, NULL);

    //if fail, do message and exit
    if(check != 0) {
        perror("udp_in thread error!");
        exit(1);
    }
}

void udp_in_cancel(void) {
    pthread_cancel(threadPID);
}

void udp_in_shutdown(void) {
    pthread_join(threadPID, NULL);
}