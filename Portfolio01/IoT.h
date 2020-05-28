#ifndef IOT_H
#define IOT_H

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

//Sensor
#define DHTPIN 0

//LED
#define LED_SIZE 3
#define LEDALARM 25
#define LEDPIN02 40
#define LEDBUTTON 29

//Button
#define BUTTONPIN01 21
#define BUTTONPIN02 22
#define BUTTONPIN03 23

#endif
