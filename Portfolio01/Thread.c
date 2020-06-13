#include "Thread.h"

int client_count = 0;
int client_sockets[MAX_CLIENT] = {-1,};
pthread_mutex_t mutex_handle_client;

static sem_t sem_DHT11;
static sem_t sem_Button;

int flag = 0;
char humidity[17] = {0,};
char temperature[17] = {0,};
char set_max_temperature[5] = {"27.0"};
char set_min_temperature[5] = {"17.0"};
char set_max_humidity[5] = {"87.0"};
char set_min_humidity[5] = {"60.0"};

void * listen_client(void * server_socket)
{
	int client_socket = 0;
	struct sockaddr_in client_address;
	memset(&client_address, 0x00, sizeof(struct sockaddr_in));
	socklen_t client_address_size = 0;
	pthread_t thread_handle_client_id;
	memset(&thread_handle_client_id, 0x00, sizeof(pthread_t));

	for(;;)
	{
		client_address_size = sizeof(client_address);
		client_socket = accept(*((int *)server_socket), (struct sockaddr *)&client_address, &client_address_size);

		pthread_mutex_lock(&mutex_handle_client);
		client_sockets[client_count++] = client_socket;
		pthread_mutex_unlock(&mutex_handle_client);

		pthread_create(&thread_handle_client_id, NULL, handle_client, (void *)&client_socket);
		pthread_detach(thread_handle_client_id);
		fprintf(stdout, "Connected client%d IP,Port : %s, %d\n", client_socket, inet_ntoa(client_address.sin_addr), client_address.sin_port);
	}

	return NULL;
}

void * handle_client(void * arg)
{
	int client_socket = *((int *)arg);
	char message[BUFFER_SIZE] = {0,};

	for(;;)
	{
		read(client_socket, message, sizeof(message));
		if(strncmp(message, "1", 1) == 0)
		{
			memset(message, 0x00, strlen(message));
			strcat(message, "1 ");
			strcat(message, temperature);
			strcat(message, " ");
			strcat(message, humidity);
		}
		else if(strncmp(message, "2", 1) == 0)
		{
			memset(message, 0x00, strlen(message));
			strcat(message, "2 ");
			strcat(message, temperature);
			strcat(message, " ");
			strcat(message, humidity);
		}
		else if(strncmp(message, "3", 1) == 0)
		{
		}
		else if(!strncmp(message, "q", 1) || !strncmp(message, "Q", 1) || !strncmp(message, "", 1))
		{
			fprintf(stdout, "Disconnected Client%d\n", client_socket);
			break;
		}
		else
		{
			fprintf(stdout, "클라이언트가 메세지를 잘 못보네요..?\n");
			fprintf(stdout, "client : %s\n", message);
			
			strcpy(message, "잘 확인하시고 다시 보내보시게..");
			send_message(message, strlen(message), client_socket);

			continue;
		}

		send_message(message, strlen(message), client_socket);
		memset(message, 0x00, strlen(message));
	}

	pthread_mutex_lock(&mutex_handle_client);
	for(int i = 0; i < client_count; i++)
	{
		if(client_socket == client_sockets[i])
		{
			while(i++ < client_count - 1)
			{
				client_sockets[i] = client_sockets[i + 1];
			}
			
			break;
		}
	}

	client_count--;
	close(client_socket);
	pthread_mutex_unlock(&mutex_handle_client);

	return NULL;
}

void send_message(char * message, int length, int client_socket)
{
	fprintf(stdout, "send_message to client%d : %s\n", client_socket, message);

	pthread_mutex_lock(&mutex_handle_client);
//	for(int i = 0; i < client_count; i++)
//	{
//		write(client_sockets[i], message, length);
//	}
	write(client_socket, message, length);

	pthread_mutex_unlock(&mutex_handle_client);

	return ;
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
			typeln("Max Temp Set");
			lcdLoc(LINE2);
			typeln(set_max_temperature);
			typeln("'C");

		}
		else if(flag == 2)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Min Temp Set");
			lcdLoc(LINE2);
			typeln(set_min_temperature);
			typeln("'C");
		}
		else if(flag == 3)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Max Humid Set");
			lcdLoc(LINE2);
			typeln(set_max_humidity);
			typeln("%");
		}
		else if(flag == 4)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Min Humid Set");
			lcdLoc(LINE2);
			typeln(set_min_humidity);
			typeln("%");
		}

		sem_wait(&sem_DHT11);
		sem_wait(&sem_Button);

		delay(20);
		ClrLcd();
		lcdLoc(LINE1);
		typeln("Temper: ");
		typeln(temperature);
		typeln("'C");
		lcdLoc(LINE2);
		typeln("Humid: ");
		typeln(humidity);
		typeln("%");
	}

	return NULL;
}

void * thread_DHT11(void * arg)
{
	for(;;)
	{
		read_dht11_data(humidity, temperature, set_max_temperature, set_min_temperature, set_max_humidity, set_min_humidity);
		delay(2000);

		sem_post(&sem_DHT11);
	}

	return NULL;
}

void * thread_Button(void * arg)
{
	for(;;)
	{
		if(digitalRead(BUTTONPIN01) == 0)
		{
			sem_wait(&sem_Button);
			flag = push_flag_Button(flag, BUTTONPIN01);

			digitalWrite(LEDBUTTON, LOW);
		}
		else if(digitalRead(BUTTONPIN02) == 0 && flag > 0)
		{
			sem_wait(&sem_Button);
			if(flag == 1)
				push_increase_Button(BUTTONPIN02, set_max_temperature);
			else if(flag == 2)
				push_increase_Button(BUTTONPIN02, set_min_temperature);
			else if(flag == 3)
				push_increase_Button(BUTTONPIN02, set_max_humidity);
			else if(flag == 4)
				push_increase_Button(BUTTONPIN02, set_min_humidity);
			
			digitalWrite(LEDBUTTON, LOW);
		}
		else if(digitalRead(BUTTONPIN03) == 0 && flag > 0)
		{
			sem_wait(&sem_Button);
			if(flag == 1)
				push_decrease_Button(BUTTONPIN03, set_max_temperature);
			else if(flag == 2)
				push_decrease_Button(BUTTONPIN03, set_min_temperature);
			else if(flag == 3)
				push_decrease_Button(BUTTONPIN03, set_max_humidity);
			else if(flag == 4)
				push_decrease_Button(BUTTONPIN03, set_min_humidity);

			digitalWrite(LEDBUTTON, LOW);
		}
		else
		{
			digitalWrite(LEDBUTTON, HIGH);
		}

		sem_post(&sem_Button);
	}

	return NULL;
}
