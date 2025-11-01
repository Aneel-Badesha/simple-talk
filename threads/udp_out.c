#include "udp_out.h"
#include "udp_in.h"
#include "keyb_in.h"
#include "screen_out.h"
#include "list.h"

// thread static vars
static pthread_t threadPID;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t perm = PTHREAD_COND_INITIALIZER;

// static vars for server setup info + msg
static char* PORT;
static List* send_list;
static char* remote_name;
static char* input;
static int send_socket;
static struct addrinfo* serv;
static struct addrinfo hints, *p;


void *udp_out_thread(void *arg) {

    // initializes hints struct, sets the addr fam to IPV4, sets socket type to UDP
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //get address info for the remote host and port
    int check = getaddrinfo(remote_name, PORT, &hints, &serv);
    if (check != 0) {
        fprintf(stderr, "Error getting address info\n");
        exit(1);
    }

    for (p = serv; p != NULL; p = p->ai_next) {
    send_socket = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);

    if (send_socket < 0) {
        perror("Issue creating socket endpoint\n");
        continue;
    }
    break;
}
    if (p == NULL) {
        printf("no sock\n");
        exit(1);
    }

    while (true) {
        pthread_mutex_lock(&lock);
        {
            // when msg is rdy to send, get it from send list
            pthread_cond_wait(&perm, &lock);
            input = List_trim(send_list);
        }
        pthread_mutex_unlock(&lock);

        if (input == NULL) {
            printf("message empty\n");
            continue;
        }
        // send msg to remote server
        int mem_sent = sendto(send_socket, input, strlen(input), 0, p->ai_addr, p->ai_addrlen);

        if (mem_sent < 0) {
            perror("error sending data\n");
            exit(1);
        }
        // if termination character is sent, cancel all threads
        if (strcmp(input, "!\n") == 0) {
            udp_in_cancel();
            udp_out_cancel();
            keyb_in_cancel();
            screen_out_cancel();
        }
        free(input);
    }
    return NULL;
}

void udp_out_init(List* list, char* remoteport, char* name)   {
    send_list = list;
    PORT = remoteport;
    remote_name = name;

    int check = pthread_create(&threadPID, NULL, udp_out_thread, NULL);
    if (check != 0) {
        perror("udp_out thread error\n");
        exit(1);
    }
}

void udp_out_start(void) {
    pthread_mutex_lock(&lock);
    {
        pthread_cond_signal(&perm);
    }
    pthread_mutex_unlock(&lock);

}

//sends cancellation request to the keyb_in thread
void udp_out_cancel(void)   {
    pthread_cancel(threadPID);
}

//shutdown the thread using pthread_join
void udp_out_shutdown(void) {
    pthread_join(threadPID, NULL);
}