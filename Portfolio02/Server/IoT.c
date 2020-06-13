#include "IoT.h"
#include "Thread.h"

extern pthread_mutex_t mutex_handle_client;

int main(int argc, char *argv[])
{
	int server_socket = 0;
	struct sockaddr_in server_address;
	memset(&server_address, 0x00, sizeof(struct sockaddr_in));
	pthread_t thread_listen_client;
	memset(&thread_listen_client, 0x00, sizeof(pthread_t));

	if(argc != 2)
	{
		fprintf(stderr, "Usage : %s <PORT>\n", argv[0]);
		exit(1);
	}

	server_socket = socket(PF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		error_handling("bind() error");

	if(listen(server_socket, 5) == -1)
		error_handling("listen() error");

	if(pthread_create(&thread_listen_client, NULL, listen_client, (void *)&server_socket) != 0)
		error_handling("pthread_create(listen_client) error");
	pthread_mutex_init(&mutex_handle_client, NULL);

	if(pthread_join(thread_listen_client, NULL) != 0)
		error_handling("pthread_detach(listen_client) error");

	pthread_mutex_destroy(&mutex_handle_client);

	close(server_socket);

	return 0;
}
