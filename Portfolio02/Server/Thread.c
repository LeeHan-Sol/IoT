#include "Thread.h"

pthread_mutex_t mutex_handle_client;
<<<<<<< HEAD
List * list_client_fd = NULL;
=======
List * list = NULL;
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe

void * listen_client(void * server_socket)
{
	int client_socket = 0;
	
<<<<<<< HEAD
	list_client_fd = createList();
=======
	list = createList();
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe

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
<<<<<<< HEAD
		list_client_fd->pushRear(list_client_fd, client_socket);
		pthread_mutex_unlock(&mutex_handle_client);

		pthread_create(&thread_handle_client, NULL, handle_client, (void *)list_client_fd->tail);
=======
		list->pushRear(list, client_socket);
		pthread_mutex_unlock(&mutex_handle_client);

		pthread_create(&thread_handle_client, NULL, handle_client, (void *)list->tail);
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe
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
<<<<<<< HEAD
		if(strcmp(message_receive, "q") == 0 || strcmp(message_receive, "Q") == 0 || strcmp(message_receive, "") == 0)
=======
		if(strcmp(message_receive, "q") == 0 || strcmp(message_receive, "Q") == 0)
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe
		{
			fprintf(stdout, "Disconnected Client%d\n", client_socket);
			break;
		}
		else
		{
<<<<<<< HEAD
			fprintf(stdout, "%s[%d]\n", message_receive, strlen(message_receive));
			int length = strlen(message_receive);
			send_message(message_receive, length, client_socket);
=======
			fprintf(stdout, "%s\n", message_receive);
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe
		}

		memset(message_receive, 0x00, strlen(message_receive));
	}

	pthread_mutex_lock(&mutex_handle_client);
<<<<<<< HEAD
	list_client_fd->popItem(list_client_fd, node_client);
=======
	list->popItem(list, node_client);
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe
	pthread_mutex_unlock(&mutex_handle_client);
	close(client_socket);

	return NULL;
}

void send_message(const char * message, int length, int client_socket)
{
	fprintf(stdout, "message_send to client%d : %s\n", client_socket, message);
<<<<<<< HEAD
	for(int i = 0; i < length; i++)
	{
		fprintf(stdout, "%d ", *((char *)message + i));
	}
=======
>>>>>>> 934ac3d6ef1c9e06d76cf0cebd7627f74d466ebe

	pthread_mutex_lock(&mutex_handle_client);
	write(client_socket, message, length);
	pthread_mutex_unlock(&mutex_handle_client);

	return ;
}
