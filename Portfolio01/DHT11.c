#include "DHT11.h"

int dht11_dat[5] = {0, };

void read_dht11_dat(char * humidity, char * temperature)
{

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0;

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

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
      dht11_dat[j / 8] <<= 1;
      if (counter > 50) dht11_dat[j / 8] |= 1;
      j++;
    }
  }

  for(int i = 0; i < 5; i++)
  {
	  fprintf(stdout, "data[%d] : %x\n", i, dht11_dat[i]);
  }

  fprintf(stdout, "data[0~3] : %x\n", ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff));

  if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff))) 
  {
    printf("humidity = %d.%d%% Temperature = %d.%d`C \n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]);

	sprintf(humidity, "humid : %d.%d%%", dht11_dat[0], dht11_dat[1]);
	sprintf(temperature, "Temp : %d.%d'C", dht11_dat[2], dht11_dat[3]);
  }
  else 
	  printf("Data get failed\n");

  return ;
}
