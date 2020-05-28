#include "Button.h"

void init_Button()
{
	pinMode(BUTTONPIN01, INPUT);
	pinMode(BUTTONPIN02, INPUT);
	pinMode(BUTTONPIN03, INPUT);
}

int push_flag_Button(int flag, const int pin)
{
	int prev_state = 0;
	delay(20);

	if(prev_state != digitalRead(pin))
	{
		flag++;
		delay(20);
	}

	if(flag > 1) flag = 0;

	return flag;
}

char * push_increase_Button(const int pin, char * set_temperature)
{
	double result = atof(set_temperature);

	int prev_state = 0;
	delay(20);

	if(prev_state != digitalRead(pin))
	{
		result += 0.1;
		sprintf(set_temperature, "%0.1f", result);

		delay(20);
		fprintf(stdout, "result : %f\n", result);
	}

	return set_temperature;
}

char * push_decrease_Button(const int pin, char * set_temperature)
{
	double result = atof(set_temperature);

	int prev_state = 0;
	delay(20);

	if(prev_state != digitalRead(pin))
	{
		result -= 0.1;
		sprintf(set_temperature, "%0.1f", result);

		delay(20);
		fprintf(stdout, "result : %f\n", result);
	}

	return set_temperature;
}
