#include "LED.h"

void init_LED()
{
	int led[LED_SIZE] = {LEDALARM, LEDPIN02, LEDBUTTON};
	for(int i = 0; i < LED_SIZE; i++)
	{
		pinMode(led[i], OUTPUT);
		digitalWrite(led[i], HIGH);
	}

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

