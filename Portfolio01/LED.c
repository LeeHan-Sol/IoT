#include "LED.h"

void initLED()
{
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, HIGH);

	return ;
}

void LED_ON()
{
	digitalWrite(LEDPIN, LOW);
	fputs("LED ON\n", stdout);

	return ;
}

void LED_OFF()
{
	digitalWrite(LEDPIN, HIGH);
	fputs("LED OFF\n", stdout);

	return ;
}

