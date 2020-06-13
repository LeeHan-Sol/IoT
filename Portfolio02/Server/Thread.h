#ifndef THREAD_H
#define THREAD_H

#include "IoT.h"
#include "List.h"

#define BUFFER_SIZE 1024

void * listen_client(void *);
void * handle_client(void *);
void send_message(const char *, int, int);

#endif
