#include "Thread.h"

pthread_mutex_t mutex_handle_client;
List * list = NULL;

void * listen_client(void * server_socket)
{
	int client_socket = 0;
	
	list = createList();

	struct sockaddr_in client_address;
	memset(&client_address, 0x00, sizeof(struct sockaddr_in));
	socklen_t client_address_size = 0;
	pthread_t thread_handle_client;
	memset(&thread_handle_client, 0x00, sizeof(pthread_t));

	for(;;)
	{
		client_address_size = sizeof(client_address);
		client_socket = accept(*((int *)server_socket), (struct sockaddr *)&client_address, &client_address_size);

		pthread_mutex_lock(&mutex_handle_client);
		list->pushRear(list, client_socket);
		pthread_mutex_unlock(&mutex_handle_client);

		pthread_create(&thread_handle_client, NULL, handle_client, (void *)list->tail);
		pthread_detach(thread_handle_client);
		fprintf(stdout, "Connected client%d IP, PORT : %s, %d\n", client_socket, inet_ntoa(client_address.sin_addr), client_address.sin_port);
	}
	
	return NULL;
}

void * handle_client(void * arg)
{
	Node * node_client = (Node *)arg;
	int client_socket = *((int *)node_client->data);
	char message_receive[BUFFER_SIZE] = {0,};

	for(;;)
	{
		read(client_socket, message_receive, sizeof(message_receive));
		if(strcmp(message_receive, "q") == 0 || strcmp(message_receive, "Q") == 0 || strcmp(message_receive, "") == 0)
		{
			fprintf(stdout, "Disconnected Client%d\n", client_socket);
			break;
		}
		else
		{
			fprintf(stdout, "%s[%d]\n", message_receive, strlen(message_receive));
			int length = strlen(message_receive);
			send_message(message_receive, length, client_socket);
		}

		memset(message_receive, 0x00, strlen(message_receive));
	}

	pthread_mutex_lock(&mutex_handle_client);
	list->popItem(list, node_client);
	pthread_mutex_unlock(&mutex_handle_client);
	close(client_socket);

	return NULL;
}

void send_message(const char * message, int length, int client_socket)
{
	fprintf(stdout, "message_send to client%d : %s\n", client_socket, message);
	for(int i = 0; i < length; i++)
	{
		fprintf(stdout, "%d ", *((char *)message + i));
	}

	pthread_mutex_lock(&mutex_handle_client);
	write(client_socket, message, length);
	pthread_mutex_unlock(&mutex_handle_client);

	return ;
}
