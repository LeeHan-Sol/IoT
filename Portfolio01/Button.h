#ifndef BUTTON_H
#define BUTTON_H

#include "IoT.h"

void init_Button();
int push_flag_Button(int, const int);
char * push_increase_Button(const int, char *);
char * push_decrease_Button(const int, char *);

#endif
