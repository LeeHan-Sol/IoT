#ifndef THREAD_H
#define THREAD_H

#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"
#include "Button.h"
#include "Error.h"

void * listen_client(void *);
void * handle_client(void *);
void send_message(char *, int, int);

void * thread_TextLCD(void *);
void * thread_DHT11(void *);
void * thread_Button(void *);

static sem_t sem_DHT11;
static sem_t sem_Button;

#endif
