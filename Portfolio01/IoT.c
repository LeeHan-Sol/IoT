#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"

int main(void)
{
	if (wiringPiSetup() == -1) exit(1);

	fd = wiringPiI2CSetup(I2C_ADDR);
	//printf("fd = %d ", fd);
	lcd_init(); // setup LCD

	char * temperature = (char *)malloc(sizeof(char) * 16);
	char * humidity = (char *)malloc(sizeof(char) * 16);
	memset(temperature, 0x00, sizeof(char) * 16);
	memset(humidity, 0x00, sizeof(char) * 16);

	while (1) 
	{
		read_dht11_dat(humidity, temperature);
		delay(2500);

		delay(2000);
		ClrLcd();
		lcdLoc(LINE1);
		typeln(temperature);
		lcdLoc(LINE2);
		typeln(humidity);
	}

	free(temperature);
	free(humidity);

	return 0 ;
}
