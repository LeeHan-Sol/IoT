#include "Thread.h"

//extern int client_count;
//extern int client_sockets[MAX_CLIENT];
extern pthread_mutex_t mutex_handle_client;

//extern int flag;
//extern char humidity[17];
//extern char temperature[17];
//extern char set_max_temperature[5];
//extern char set_min_temperature[5];
//extern char set_max_humidity[5];
//extern char set_min_humidity[5];

//extern sem_t sem_DHT11;
//extern sem_t sem_Button;

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
