#ifndef IOT_H
#define IOT_H

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define DHTPIN 0

#define LEDALARM 25
#define LEDPIN02 40

#define BUTTONPIN01 21
#define BUTTONPIN02 22
#define BUTTONPIN03 23
#define BUTTONLEDPIN 29

#endif
