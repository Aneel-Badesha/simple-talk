#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "udp_out.h"
#include "keyb_in.h"
#include "udp_in.h"
#include "screen_out.h"

int main(int argc, char* argv[]) {

    if (argc != 4) {
        printf("missing main parameters");
        return -1;
    }

    int localport = atoi(argv[1]);
    char* remote_name = argv[2];
    char* remoteport = argv[3];

    List* to_send = List_create();
    List* receive = List_create();

    keyb_in_init(to_send);
    udp_out_init(to_send, remoteport, remote_name);
    udp_in_init(receive, localport);
    screen_out_init(receive);

    udp_in_shutdown();
    udp_out_shutdown();
    keyb_in_shutdown();
    screen_out_shutdown();

    return 0;
}