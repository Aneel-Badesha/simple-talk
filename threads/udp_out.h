#ifndef UDP_OUT_H
#define UDP_OUT_H

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

#define MESSAGE_SIZE 1024

void *udp_out_thread(void *arg);
void udp_out_start(void);
void udp_out_init(List* list, char* remoteport, char* name);
void udp_out_cancel(void);
void udp_out_shutdown(void);

#endif