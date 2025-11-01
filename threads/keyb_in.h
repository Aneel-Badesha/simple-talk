#ifndef KEYB_IN_H
#define KEYB_IN_H

#include "list.h"

void *keyb_in_thread(void *arg);
void keyb_in_init(List* list);
void keyb_in_cancel(void);
void keyb_in_shutdown(void);

#endif