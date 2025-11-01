#ifndef SCREEN_OUT_H
#define SCREEN_OUT_H

#include "list.h"

void* screen_out_thread(void* arg);
void screen_out_init(List* list);
void screen_out_start();
void screen_out_cancel(void);
void screen_out_shutdown(void);

#endif