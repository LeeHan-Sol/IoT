#include "LED.h"

void init_LED()
{
	pinMode(LEDALARM, OUTPUT);
	digitalWrite(LEDALARM, HIGH);

	return ;
}

void LED_ON(const int pin)
{
	digitalWrite(pin, LOW);
	fputs("LED ON\n", stdout);

	return ;
}

void LED_OFF(const int pin)
{
	digitalWrite(pin, HIGH);
	fputs("LED OFF\n", stdout);

	return ;
}

