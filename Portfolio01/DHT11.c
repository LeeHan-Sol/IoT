#include "DHT11.h"

int dht11_data[5] = {0, };

void read_dht11_data(char * humidity, char * temperature)
{

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0;

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

  for(int i = 0; i < 5; i++)
  {
	  fprintf(stdout, "data[%d] : %x\n", i, dht11_data[i]);
  }

  fprintf(stdout, "data[0~3] : %x\n", ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff));

  if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xff))) 
  {
    printf("humidity = %d.%d%% Temperature = %d.%d`C \n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);

	if(dht11_data[3] >= 5) LED_ON();
	else LED_OFF();

	sprintf(humidity, "humid: %d.%d%%", dht11_data[0], dht11_data[1]);
	sprintf(temperature, "Temp: %d.%d'C", dht11_data[2], dht11_data[3]);
  }
  else 
	  printf("Data get failed\n");

  return ;
}
