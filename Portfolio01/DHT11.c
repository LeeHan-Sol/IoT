#include "DHT11.h"

int dht11_data[5] = {0, };
int flag_data_status = 0;

void read_dht11_data(char * humidity, char * temperature, char * set_max_temperature, char * set_min_temperature, char * set_max_humidity, char * set_min_humidity)
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0;

	double d_temperature = 0.0;
	double d_humidity = 0.0;

	double d_set_max_temperature = atof(set_max_temperature);
	double d_set_min_temperature = atof(set_min_temperature);
	double d_set_max_humidity = atof(set_max_humidity);
	double d_set_min_humidity = atof(set_min_humidity);

	dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;

	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(18);

	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(40);
	pinMode(DHTPIN, INPUT);

	for(int i = 0; i < MAXTIMINGS; i++) 
	{
		counter = 0;
		while (digitalRead(DHTPIN) == laststate) 
		{ 
			counter++;
			delayMicroseconds(1);
			if (counter == 255) break;
		}

		laststate = digitalRead(DHTPIN) ;

		if (counter == 255) break ; // if while breaked by timer, break for

		if ((i >= 4) && (i % 2 == 0)) 
		{
			dht11_data[j / 8] <<= 1;
			if (counter > 50) dht11_data[j / 8] |= 1;
			j++;
		}
	}

//	for(int i = 0; i < 5; i++)
//	{
//		fprintf(stdout, "data[%d] : %x\n", i, dht11_data[i]);
//	}

//	fprintf(stdout, "data[0~3] : %x\n", ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff));

	if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff))) 
	{
		if(flag_data_status == 1)
		{
			fprintf(stdout, "\n");
		}
		sprintf(humidity, "%d.%d",dht11_data[0], dht11_data[1]);
		sprintf(temperature, "%d.%d", dht11_data[2], dht11_data[3]);

	//	display_dht11_data(temperature, humidity, set_max_temperature, set_min_temperature, set_max_humidity, set_min_humidity);

		d_temperature = atof(temperature);
		d_humidity = atof(humidity);

		if(d_temperature >= d_set_max_temperature || d_temperature <= d_set_min_temperature || d_humidity >= d_set_max_humidity || d_humidity <= d_set_min_humidity) 
			LED_ON(LEDALARM);
		else 
			LED_OFF(LEDALARM);

		flag_data_status = 0;
	}
	else 
	{
		if(flag_data_status == 0)
		{
			flag_data_status = 1;
			fprintf(stdout, "\tDHT11 데이터 수신이 좋지 않습니다.");
		}
		else
		{
			fprintf(stdout, ".");
		}
	}

	return ;
}

//void display_dht11_data(char * temperature, char * humidity, char * set_max_temperature, char * set_min_temperature, char * set_max_humidity, char * set_min_humidity)
//{
//	fprintf(stdout, "      ----------------------\n");
//	fprintf(stdout, "      |   온도   |   습도  |\n");
//	fprintf(stdout, "----------------------------\n");
//	fprintf(stdout, " 현재 |  %s'C  |  %s%%  |\n", temperature, humidity);
//	fprintf(stdout, "----------------------------\n");
//	fprintf(stdout, " 최소 |  %s'C  |  %s%%  |\n", set_min_temperature, set_min_humidity);
//	fprintf(stdout, " 최대 |  %s'C  |  %s%%  |\n", set_max_temperature, set_max_humidity);
//	fprintf(stdout, "----------------------------\n");
//}
