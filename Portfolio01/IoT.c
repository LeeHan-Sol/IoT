#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"
#include "Button.h"

void * thread_TextLCD(void *);
void * thread_DHT11(void *);
void * thread_Button(void *);

int flag = 0;
char humidity[17] = {0,};
char temperature[17] = {0,};
char set_temperature[5] = {"23.0"};

static sem_t sem_DHT11;
static sem_t sem_Button;

int main(void)
{
	pthread_t * thread_TextLCD_id, * thread_DHT11_id, * thread_Button_id;

	thread_TextLCD_id = (pthread_t *)malloc(sizeof(pthread_t));
	thread_DHT11_id = (pthread_t *)malloc(sizeof(pthread_t));
	thread_Button_id = (pthread_t *)malloc(sizeof(pthread_t));

	memset(thread_TextLCD_id, 0x00, sizeof(pthread_t));
	memset(thread_DHT11_id, 0x00, sizeof(pthread_t));
	memset(thread_Button_id, 0x00, sizeof(pthread_t));

	sem_init(&sem_DHT11, 0, 0);
	sem_init(&sem_Button, 0, 1);

	if (wiringPiSetup() == -1) exit(1);

	fd = wiringPiI2CSetup(I2C_ADDR);
	lcd_init(); // setup LCD
	init_Button();
	init_LED();

	if(pthread_create(thread_TextLCD_id, NULL, thread_TextLCD, NULL) != 0)
	{
		fputs("pthread_create(TextLCD) error\n", stdout);
		return -1;
	}
	if(pthread_create(thread_DHT11_id, NULL, thread_DHT11, NULL) != 0)
	{
		fputs("pthread_create(DHT11) error\n", stdout);
		return -1;
	}
	if(pthread_create(thread_Button_id, NULL, thread_Button, NULL) != 0)
	{
		fputs("pthread_create(DHT11) error\n", stdout);
		return -1;
	}

	if(pthread_join(*thread_TextLCD_id, NULL) != 0)
	{
		fputs("pthread_join(TextLCD) error\n", stdout);
		return -1;
	}
	if(pthread_join(*thread_DHT11_id, NULL) != 0)
	{
		fputs("pthread_join(DHT11) error\n", stdout);
		return -1;
	}
	if(pthread_join(*thread_Button_id, NULL) != 0)
	{
		fputs("pthread_join(Button) error\n", stdout);
		return -1;
	}

//	while (1) 
//	{
//		read_dht11_dat(humidity, temperature);
//		delay(2500);
//
//		delay(2000);
//		ClrLcd();
//		lcdLoc(LINE1);
//		typeln(temperature);
//		lcdLoc(LINE2);
//		typeln(humidity);
//	}

//	fputs("end\n", stdout);
	sem_destroy(&sem_DHT11);
	sem_destroy(&sem_Button);

	free(thread_TextLCD_id);
	free(thread_DHT11_id);
	free(thread_Button_id);

	return 0 ;
}

void * thread_TextLCD(void * arg)
{
	for(;;)
	{
		if(strncmp(temperature, "\0", 1) == 0)
		{
			fputs("\tplz wait for setting\n", stdout);
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Plz Wait for");
			lcdLoc(LINE2);
			typeln("      setting...");
		}
		else if(flag == 1)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Alarming temper");
			lcdLoc(LINE2);
			typeln(set_temperature);

		}
		else if(flag == 2)
		{
		}
		else if(flag == 3)
		{
		}

		sem_wait(&sem_DHT11);
		sem_wait(&sem_Button);

		delay(2000);
		ClrLcd();
		lcdLoc(LINE1);
		typeln(temperature);
//		typeln("Hello World!");
		lcdLoc(LINE2);
		typeln(humidity);
	}

	return NULL;
}

void * thread_DHT11(void * arg)
{
	for(;;)
	{
		fprintf(stdout, "\t\tDHT11\n");

		read_dht11_data(humidity, temperature, set_temperature);
		delay(2000);

		sem_post(&sem_DHT11);
	}

	return NULL;
}

void * thread_Button(void * arg)
{
	fprintf(stdout, "flag : %d\n", flag);
	for(;;)
	{
		if(digitalRead(BUTTONPIN01) == 0)
		{
			fputs("\t\t\t버튼이 눌렸다!.\n", stdout);
			sem_wait(&sem_Button);
			flag = push_flag_Button(flag, BUTTONPIN01);

			fprintf(stdout, "flag : %d\n", flag);
			digitalWrite(BUTTONLEDPIN, LOW);
		}
		else if(digitalRead(BUTTONPIN02) == 0 && flag == 1)
		{
			fputs("\t\t\t버튼이 눌렸다!.\n", stdout);
			sem_wait(&sem_Button);
			push_increase_Button(BUTTONPIN02, set_temperature);

			digitalWrite(BUTTONLEDPIN, LOW);
		}
		else if(digitalRead(BUTTONPIN03) == 0 && flag == 1)
		{
			fputs("\t\t\t버튼이 눌렸다!.\n", stdout);
			sem_wait(&sem_Button);
			push_decrease_Button(BUTTONPIN03, set_temperature);

			digitalWrite(BUTTONLEDPIN, LOW);
		}
		else
		{
			digitalWrite(BUTTONLEDPIN, HIGH);
		}

		sem_post(&sem_Button);
	}

	return NULL;
}
