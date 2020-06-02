#ifndef THREAD_H
#define THREAD_H

void * listen_client(void *);
void * handle_client(void *);

void * thread_TextLCD(void *);
void * thread_DHT11(void *);
void * thread_Button(void *);

static sem_t sem_DHT11;
static sem_t sem_Button;

#endif
