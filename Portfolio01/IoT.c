#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"

void * thread_TextLCD(void *);
void * thread_DHT11(void *);

char * humidity = NULL;
char * temperature = NULL;

static sem_t sem_one;
static sem_t sem_two;

int main(void)
{
	pthread_t * thread_TextLCD_id, * thread_DHT11_id;
	thread_TextLCD_id = (pthread_t *)malloc(sizeof(pthread_t));
	thread_DHT11_id = (pthread_t *)malloc(sizeof(pthread_t));
	memset(thread_TextLCD_id, 0x00, sizeof(pthread_t));
	memset(thread_DHT11_id, 0x00, sizeof(pthread_t));

	sem_init(&sem_one, 0, 0);
	sem_init(&sem_two, 0, 1);

	if (wiringPiSetup() == -1) exit(1);

//pinMode(DHTPIN, OUTPUT);
//digitalWrite(DHTPIN, LOW);
//delay(18);
//
//digitalWrite(DHTPIN, HIGH);
//delayMicroseconds(40);
//pinMode(DHTPIN, INPUT);

	fd = wiringPiI2CSetup(I2C_ADDR);
	lcd_init(); // setup LCD

	temperature = (char *)malloc(sizeof(char) * 16);
	humidity = (char *)malloc(sizeof(char) * 16);
	memset(temperature, 0x00, sizeof(char) * 16);
	memset(humidity, 0x00, sizeof(char) * 16);

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

	if(pthread_join(*thread_DHT11_id, NULL) != 0)
	{
		fputs("pthread_join(DHT11) error\n", stdout);
		return -1;
	}
	if(pthread_join(*thread_TextLCD_id, NULL) != 0)
	{
		fputs("pthread_join(TextLCD) error\n", stdout);
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
	free(temperature);
	free(humidity);

	sem_destroy(&sem_one);
	sem_destroy(&sem_two);

	free(thread_TextLCD_id);
	free(thread_DHT11_id);

	return 0 ;
}

void * thread_TextLCD(void * arg)
{
	sem_wait(&sem_one);
	for(;;)
	{
		fprintf(stdout, "\t\tthread_TextLCD\n\t\t\thumidity, temperature : %p, %p\n", humidity, temperature);

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

		read_dht11_dat(humidity, temperature);
		delay(2000);
		sem_post(&sem_one);
	}
	return NULL;
}
