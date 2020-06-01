#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"
#include "Button.h"

#define MAX_CLIENT 5
#define BUFFER_SIZE 1024

void error_handling(const char *);

void * listen_client(void *);
void * handle_client(void *);
void send_message(char *, int, int);

int client_count = 0;
int client_sockets[MAX_CLIENT] = {-1,};
pthread_mutex_t mutex_handle_client;

void * thread_TextLCD(void *);
void * thread_DHT11(void *);
void * thread_Button(void *);

int flag = 0;
char humidity[17] = {0,};
char temperature[17] = {0,};
char set_max_temperature[5] = {"27.0"};
char set_min_temperature[5] = {"17.0"};
char set_max_humidity[5] = {"87.0"};
char set_min_humidity[5] = {"60.0"};

static sem_t sem_DHT11;
static sem_t sem_Button;

int main(int argc, char * argv[])
{
	int server_socket = 0;
	struct sockaddr_in server_address;
	memset(&server_address, 0x00, sizeof(struct sockaddr_in));
	pthread_t thread_listen_client_id;
	memset(&thread_listen_client_id, 0x00, sizeof(pthread_t));

	if(argc != 2)
	{
		fprintf(stderr, "Usage : %s <PORT>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutex_handle_client, NULL);
	server_socket = socket(PF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		error_handling("bind() error");

	if(listen(server_socket, 5) == -1)
		error_handling("listen() error");

	if(pthread_create(&thread_listen_client_id, NULL, listen_client, (void *)&server_socket) != 0)
		error_handling("pthread_create(listen_client)");

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
		error_handling("pthread_create(TextLCD) error");
	if(pthread_create(thread_DHT11_id, NULL, thread_DHT11, NULL) != 0)
		error_handling("pthread_create(DHT11) error");
	if(pthread_create(thread_Button_id, NULL, thread_Button, NULL) != 0)
		error_handling("pthread_create(Button) error");


	if(pthread_detach(thread_listen_client_id) != 0)
		error_handling("pthread_join(listen_client) error");
	if(pthread_join(*thread_TextLCD_id, NULL) != 0)
		error_handling("pthread_join(textlcd) error");
	if(pthread_join(*thread_DHT11_id, NULL) != 0)
		error_handling("pthread_join(DHT11) error");
	if(pthread_join(*thread_Button_id, NULL) != 0)
		error_handling("pthread_join(Button) error");

	sem_destroy(&sem_DHT11);
	sem_destroy(&sem_Button);

	free(thread_TextLCD_id);
	free(thread_DHT11_id);
	free(thread_Button_id);

	close(server_socket);

	return 0 ;
}

void error_handling(const char * arg)
{
	fputs(arg, stderr);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

void * listen_client(void * server_socket)
{
	int client_socket = 0;
	struct sockaddr_in client_address;
	memset(&client_address, 0x00, sizeof(struct sockaddr_in));
	socklen_t client_address_size = 0;
	pthread_t thread_handle_client_id;
	memset(&thread_handle_client_id, 0x00, sizeof(pthread_t));

	while(1)
	{
		client_address_size = sizeof(client_address);
		client_socket = accept(*((int *)server_socket), (struct sockaddr *)&client_address, &client_address_size);

		pthread_mutex_lock(&mutex_handle_client);
		client_sockets[client_count++] = client_socket;
		pthread_mutex_unlock(&mutex_handle_client);

		pthread_create(&thread_handle_client_id, NULL, handle_client, (void *)&client_socket);
		pthread_detach(thread_handle_client_id);
		fprintf(stdout, "Connected client%d IP : %s\n", client_socket, inet_ntoa(client_address.sin_addr));
	}

	return NULL;
}

void * handle_client(void * arg)
{
	int client_socket = *((int *)arg);
	char message[BUFFER_SIZE] = {0,};

	while(1)
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
	pthread_mutex_unlock(&mutex_handle_client);
	close(client_socket);

	return NULL;
}

void send_message(char * message, int length, int client_socket)
{
	fprintf(stdout, "send_message(%d) : %s\n", client_socket, message);

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
		fprintf(stdout, "\t\tDHT11\n");

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
