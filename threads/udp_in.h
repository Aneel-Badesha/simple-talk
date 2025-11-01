#ifndef UDP_IN_H
#define UDP_IN_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "list.h"
#include "screen_out.h"

#define MESSAGE_SIZE 1024

void *udp_in_thread(void *arg);
void udp_in_init(List* list, int local_port);
void udp_in_cancel(void);
void udp_in_shutdown(void);

#endif