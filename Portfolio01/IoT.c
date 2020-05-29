#include "IoT.h"
#include "DHT11.h"
#include "TextLCD.h"
#include "Button.h"

#define MAX_CLIENT 5
#define BUFFER_SIZE 1024

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
char set_max_temperature[5] = {"23.0"};
char set_min_temperature[5] = {"17.0"};
char set_max_humidity[5] = {"87.0"};
char set_min_humidity[5] = {"87.0"};

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
	{
		fprintf(stderr, "bind() error\n");
		exit(1);
	}

	if(listen(server_socket, 5) == -1)
	{
		fprintf(stderr, "listen() error\n");
		exit(1);
	}

	if(pthread_create(&thread_listen_client_id, NULL, listen_client, (void *)&server_socket) != 0)
	{
		fprintf(stderr, "pthread_create(listen_client)\n");
		exit(1);
	}

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
		fputs("pthread_create(Button) error\n", stdout);
		return -1;
	}


	if(pthread_detach(thread_listen_client_id) != 0)
	{
		fputs("pthread_join(listen_client) error\n", stdout);
		return -1;
	}

	if(pthread_join(*thread_TextLCD_id, NULL) != 0)
	{
		fputs("pthread_join(textlcd) error\n", stdout);
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

	sem_destroy(&sem_DHT11);
	sem_destroy(&sem_Button);

	free(thread_TextLCD_id);
	free(thread_DHT11_id);
	free(thread_Button_id);

	close(server_socket);

	return 0 ;
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
		fprintf(stdout, "Connected client IP : %s\n", inet_ntoa(client_address.sin_addr));
	}

	return NULL;
}

void * handle_client(void * arg)
{
	int client_socket = *((int *)arg);
	char message[BUFFER_SIZE] = {0,};

//	while((string_length = read(client_socket, message, sizeof(message))) != 0)
	while(1)
	{
		read(client_socket, message, sizeof(message));
		if(strncmp(message, "1", 1) == 0)
		{
			memset(message, 0x00, strlen(message));
			strcpy(message, temperature);
		}
		else if(strncmp(message, "2", 1) == 0)
		{
			memset(message, 0x00, strlen(message));
			strcpy(message, humidity);
		}
		else if(strncmp(message, "3", 1) == 0)
		{
			memset(message, 0x00, strlen(message));
			strcpy(message, temperature);
			strcat(message, " ");
			strcat(message, humidity);
		}
		else if(!strncmp(message, "q", 1) || !strncmp(message, "Q", 1))
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
	fprintf(stdout, "send_message : %s\n", message);

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

		}
		else if(flag == 2)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Min Temp Set");
			lcdLoc(LINE2);
			typeln(set_max_temperature);
		}
		else if(flag == 3)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Max Humid Set");
			lcdLoc(LINE2);
			typeln(set_max_temperature);
		}
		else if(flag == 4)
		{
			ClrLcd();
			lcdLoc(LINE1);
			typeln("Miin Humid Set");
			lcdLoc(LINE2);
			typeln(set_max_temperature);
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

		read_dht11_data(humidity, temperature, set_max_temperature, set_min_temperature);
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
			digitalWrite(LEDBUTTON, LOW);
		}
		else if(digitalRead(BUTTONPIN02) == 0 && flag == 1)
		{
			fputs("\t\t\t버튼이 눌렸다!.\n", stdout);
			sem_wait(&sem_Button);
			push_increase_Button(BUTTONPIN02, set_max_temperature);

			digitalWrite(LEDBUTTON, LOW);
		}
		else if(digitalRead(BUTTONPIN03) == 0 && flag == 1)
		{
			fputs("\t\t\t버튼이 눌렸다!.\n", stdout);
			sem_wait(&sem_Button);
			push_decrease_Button(BUTTONPIN03, set_max_temperature);

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
