#include "DHT11.h"

int dht11_data[5] = {0, };

void read_dht11_data(char * humidity, char * temperature, char * set_max_temperature, char * set_min_temperature)
{

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0;

  double d_temperature = 0.0;
  double d_set_max_temperature = atof(set_max_temperature);

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

//  for(int i = 0; i < 5; i++)
//  {
//	  fprintf(stdout, "data[%d] : %x\n", i, dht11_data[i]);
//  }

//  fprintf(stdout, "data[0~3] : %x\n", ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff));

  if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff))) 
  {
    fprintf(stdout, "humidity = %d.%d%% Temperature = %d.%d`C\nset_max_temperature : %s \n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3], set_max_temperature);

	sprintf(temperature, "%d.%d", dht11_data[2], dht11_data[3]);
	d_temperature = atof(temperature);

	if(d_temperature >= d_set_max_temperature) 
		LED_ON(LEDALARM);
	else 
		LED_OFF(LEDALARM);

	sprintf(humidity, "humid: %d.%d%%", dht11_data[0], dht11_data[1]);
	sprintf(temperature, "Temper: %d.%d'C", dht11_data[2], dht11_data[3]);
  }
  else 
	  printf("Data get failed\n");

  return ;
}
