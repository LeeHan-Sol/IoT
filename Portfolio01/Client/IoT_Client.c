#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 1024
#define SETTING_SIZE 10

typedef struct Alarm
{
	char filename[100];
	
	char max_temperature[SETTING_SIZE];
	char min_temperature[SETTING_SIZE];
	char max_humidity[SETTING_SIZE];
	char min_humidity[SETTING_SIZE];
	char period[SETTING_SIZE];

	void (*init_alarm)(struct Alarm *, const char *);
	void (*set_alarm_info)(struct Alarm *);
}Alarm;

Alarm alarm_info;
void init_alarm(Alarm *, const char *);
void set_alarm_info(Alarm *);

void * user_interface(void *);
void * recv_msg(void *);
void * send_message(void *);
void error_handling(char *);

void compare_alarm_info(char *);

void display_dht11(const char *);
void display_alarm_info_service();
void display_change_alarm_info(const char * );
void display_change_alarm_info_period();

static sem_t sem_receive;
static sem_t sem_send;
static sem_t sem_alarm;

int main(int argc, char * argv[])
{
	if(argc != 4)
	{
		fprintf(stderr, "Usage : %s <IP> <PORT> <Setting File>\n", argv[0]);
		exit(1);
	}

	alarm_info.init_alarm = init_alarm;
	alarm_info.init_alarm(&alarm_info, argv[3]);
	alarm_info.set_alarm_info(&alarm_info);

	int sock = 0;
	struct sockaddr_in serv_adr;
	pthread_t snd_thread, rcv_thread, send_message_thread;
	memset(&snd_thread, 0x00, sizeof(pthread_t));
	memset(&rcv_thread, 0x00, sizeof(pthread_t));
	memset(&send_message_thread, 0x00, sizeof(pthread_t));
	void * thread_return = NULL;

	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0x00, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
	{
		error_handling("connect() error");
	}

	sem_init(&sem_receive, 0, 0);
	sem_init(&sem_send, 0, 1);
	sem_init(&sem_alarm, 0, 1);

	pthread_create(&snd_thread, NULL, user_interface, (void *)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
	pthread_create(&send_message_thread, NULL, send_message, (void *)&sock);
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	pthread_join(send_message_thread, &thread_return);

	sem_destroy(&sem_receive);
	sem_destroy(&sem_send);
	sem_destroy(&sem_alarm);

	close(sock);

	return 0;
}

void init_alarm(Alarm * alarm_info, const char * filename)
{
	memset(alarm_info, 0x00, sizeof(Alarm));
	strcpy(alarm_info->filename, filename);

	alarm_info->set_alarm_info = set_alarm_info;
}

void set_alarm_info(Alarm * alarm_info)
{
	FILE * file_alarm_info;

	for(;;)
	{	
		if(access(alarm_info->filename, F_OK) != -1)
		{
			file_alarm_info = fopen(alarm_info->filename, "r+");
			break;
		}
		else
		{
			fprintf(stdout, "새로운 파일을 생성합니다.\n");
			file_alarm_info = fopen(alarm_info->filename, "w+");
			fprintf(file_alarm_info, "27.0\n");
			fprintf(file_alarm_info, "20.0\n");
			fprintf(file_alarm_info, "87.0\n");
			fprintf(file_alarm_info, "60.0\n");
			fprintf(file_alarm_info, "1.0\n");
			
			fclose(file_alarm_info);
			
			continue;
		}
	}

	fgets(alarm_info->max_temperature, SETTING_SIZE, (FILE *)file_alarm_info);
	fgets(alarm_info->min_temperature, SETTING_SIZE, (FILE *)file_alarm_info);
	fgets(alarm_info->max_humidity, SETTING_SIZE, (FILE *)file_alarm_info);
	fgets(alarm_info->min_humidity, SETTING_SIZE, (FILE *)file_alarm_info);
	fgets(alarm_info->period, SETTING_SIZE, (FILE *)file_alarm_info);

	alarm_info->max_temperature[strlen(alarm_info->max_temperature) - 1] = 0x00;
	alarm_info->min_temperature[strlen(alarm_info->min_temperature) - 1] = 0x00;
	alarm_info->max_humidity[strlen(alarm_info->max_humidity) - 1] = 0x00;
	alarm_info->min_humidity[strlen(alarm_info->min_humidity) - 1] = 0x00;
	alarm_info->period[strlen(alarm_info->period) - 1] = 0x00;

	fclose(file_alarm_info);
}

void * user_interface(void * arg)
{
	int sock = *((int *)arg);
	char buffer[BUF_SIZE] = {0,};

	for(;;)
	{
		sem_wait(&sem_send);
		fprintf(stdout, "----------------------\n");
		fprintf(stdout, "|1. 현재 온 습도 정보|\n");
		fprintf(stdout, "----------------------\n");
		fprintf(stdout, "|2. 알림 서비스      |\n");
		fprintf(stdout, "----------------------\n");
		fprintf(stdout, "|3. 나가기           |\n");
		fprintf(stdout, "----------------------\n");
		fputs(">> ", stdout);
		fgets(buffer, BUF_SIZE, stdin);
		buffer[strlen(buffer) - 1] = 0x00;

		if(!strcmp(buffer, "q") || !strcmp(buffer, "Q"))
		{
			close(sock);
			exit(0);
		}
		else if(!strcmp(buffer, "1"))
		{
			write(sock, "1", 1);
			sem_post(&sem_receive);
		}
		else if(!strcmp(buffer, "2"))
		{
			sem_wait(&sem_alarm);
			display_alarm_info_service();
			sem_post(&sem_alarm);
		}
		else if(!strcmp(buffer, "3"))
		{
			exit(1);
		}
		else
		{
			fputs("다시 입력해주세요.\n\n", stdout);
			sem_post(&sem_send);
		}
	}
	
	return NULL;
}

void * recv_msg(void * arg)
{
	int sock = *((int *)arg);
	char buffer[BUF_SIZE] = {0,};
	int str_len;

	for(;;)	
	{
		str_len = read(sock, buffer, BUF_SIZE);
		if(str_len == -1)
			return (void *)-1;

		sem_wait(&sem_receive);
		if(!strncmp(buffer, "1", 1))
		{
			display_dht11(buffer);
			sem_post(&sem_send);
		}
		else if(!strncmp(buffer, "2", 1))
		{
			compare_alarm_info(buffer);
			sem_post(&sem_send);
		}

	}
	
	return NULL;
}

void * send_message(void * arg)
{
	int sock =*((int *)arg);
	int period = 0;

	for(;;)
	{
		period = (int)(atof(alarm_info.period) * 60);
		sleep(period);

		sem_wait(&sem_alarm);
		write(sock, "2", 1);
		sem_post(&sem_alarm);
		
		sem_post(&sem_receive);
	}

	return NULL;
}

void compare_alarm_info(char * arg)
{
	char temperature[SETTING_SIZE] ={0,};
	char humidity[SETTING_SIZE] = {0,};

	strncpy(temperature, arg + 2, 4);
	strncpy(humidity, arg + 7, 4);

	fprintf(stdout, "\n\t\t-알람-----------------\n");
	if(strcmp(temperature, alarm_info.max_temperature) >= 0)
	{
		fprintf(stdout, "\t\t|  온도가 높습니다!  |\n");
	}
	else if(strcmp(temperature, alarm_info.min_temperature) <= 0)
	{
		fprintf(stdout, "\t\t|  온도가 낮습니다!  |\n");
	}

	if(strcmp(humidity, alarm_info.max_humidity) >= 0)
	{
		fprintf(stdout, "\t\t|  습도가 높습니다!  |\n");
	}
	else if(strcmp(humidity, alarm_info.min_humidity) <= 0)
	{
		fprintf(stdout, "\t\t|  습도가 낮습니다!  |\n");
	}
	
	if(strcmp(temperature, alarm_info.max_temperature) <= 0 && strcmp(temperature, alarm_info.min_temperature) >= 0 && strcmp(humidity, alarm_info.max_humidity) <= 0 && strcmp(humidity, alarm_info.min_humidity) >= 0)
	{
		fprintf(stdout, "\t\t| 쾌적한 환경입니다. |\n");
	}

	fprintf(stdout, "\t\t----------------------\n");

	return ;
}

void error_handling(char * msg)
{
	fputs(msg, stdout);
	fputc('\n', stdout);

	exit(EXIT_FAILURE);
}

void display_dht11(const char * message)
{
	char temperature[SETTING_SIZE] = {0,};
	char humidity[SETTING_SIZE] = {0,};

	strncpy(temperature, message + 2, 4);
	strncpy(humidity, message + 7, 4);

	fprintf(stdout, "\n\t\t----------------------\n");
	fprintf(stdout, "\t\t|   온도   |   습도  |\n");
	fprintf(stdout, "\t\t----------------------\n");
	fprintf(stdout, "\t\t|  %s'C  |  %s%%  |\n", temperature, humidity);
	fprintf(stdout, "\t\t----------------------\n");
	fputc('\n', stdout);
	fputc('\n', stdout);
}

void display_alarm_info_service()
{
	char buffer[BUF_SIZE] = {0,};

	for(;;)
	{
		fprintf(stdout, "\t\t       |   온도   |   습도  |\n");
		fprintf(stdout, "\t\t-----------------------------\n");
		fprintf(stdout, "\t\t| 최소 |  %s'C  |  %s%%  |\n", alarm_info.min_temperature, alarm_info.min_humidity);
		fprintf(stdout, "\t\t| 최대 |  %s'C  |  %s%%  |\n", alarm_info.max_temperature, alarm_info.max_humidity);
		fprintf(stdout, "\t\t-----------------------------\n");
		fprintf(stdout, "\t\t알림 주기 : %s분\n", alarm_info.period);
		fputc('\n', stdout);

		fprintf(stdout, "   알림 서비스  \n");
		fprintf(stdout, "----------------\n");
		fprintf(stdout, "| 1. 온도 설정 |\n");
		fprintf(stdout, "----------------\n");
		fprintf(stdout, "| 2. 습도 설정 |\n");
		fprintf(stdout, "----------------\n");
		fprintf(stdout, "| 3. 알림 주기 |\n");
		fprintf(stdout, "----------------\n");
		fprintf(stdout, "| 4. 돌아가기  |\n");
		fprintf(stdout, "----------------\n");
		fprintf(stdout, ">> ");
		fgets(buffer, BUF_SIZE, stdin);
		buffer[strlen(buffer) - 1] = 0x00;

		if(!strcmp(buffer, "1") || !strcmp(buffer, "2") )
		{
			display_change_alarm_info(buffer);
		}
		else if(!strcmp(buffer, "3"))
		{
			display_change_alarm_info_period();
		}
		else if(!strcmp(buffer, "4"))
		{
			FILE * file_alarm_info = fopen(alarm_info.filename, "w");
			fprintf((FILE *)file_alarm_info, "%s\n", alarm_info.max_temperature);
			fprintf((FILE *)file_alarm_info, "%s\n", alarm_info.min_temperature);
			fprintf((FILE *)file_alarm_info, "%s\n", alarm_info.max_humidity);
			fprintf((FILE *)file_alarm_info, "%s\n", alarm_info.min_humidity);
			fprintf((FILE *)file_alarm_info, "%s\n", alarm_info.period);
			fputs("\n", stdout);

			sem_post(&sem_send);

			fclose(file_alarm_info);
			return ;
		}
		else
		{
			fputs("다시 입력해주세요.\n", stdout);
		}

	}
}

void display_change_alarm_info(const char * arg)
{
	char buffer[BUF_SIZE] = {0,};
	int option = atoi(arg);

	for(;;)
	{
		fprintf(stdout, "\t\t       -----------------------\n");
		fprintf(stdout, "\t\t       |   최소   |   최대   |\n");
		fprintf(stdout, "\t\t------------------------------\n");
		fprintf(stdout, "\t\t| %s도 |  %s'C  |  %s'C  |\n", option == 1 ? "온" : "습", option == 1 ? alarm_info.min_temperature : alarm_info.min_humidity, option == 1 ? alarm_info.max_temperature : alarm_info.max_humidity);
		fprintf(stdout, "\t\t------------------------------\n");

		fprintf(stdout, "\n");
		fprintf(stdout, "    알림 %s도 변경   \n", option == 1 ? "온" : "습");
		fprintf(stdout, "---------------------\n");
		fprintf(stdout, "| 1. 최대 %s도 변경 |\n", option == 1 ? "온" : "습");
		fprintf(stdout, "---------------------\n");
		fprintf(stdout, "| 2. 최소 %s도 변경 |\n", option == 1 ? "온" : "습");
		fprintf(stdout, "---------------------\n");
		fprintf(stdout, "| 3. 돌아가기       |\n");
		fprintf(stdout, "---------------------\n");
		fprintf(stdout, ">> ");
		fgets(buffer, BUF_SIZE, stdin);
		buffer[strlen(buffer) - 1] = 0x00;

		if(!strcmp(buffer, "3"))
		{
			fputs("\n", stdout);
			return ;
		}

		if(!strcmp(buffer, "1"))
		{
			fprintf(stdout, "\n최대 %s도를 변경합니다.\n", option == 1 ? "온" : "습");
			fprintf(stdout, ">> ");

			fgets(buffer, BUF_SIZE, stdin);
			buffer[strlen(buffer) - 1] = 0x00;

			if(option == 1)
				strcpy(alarm_info.max_temperature, buffer);
			else
				strcpy(alarm_info.max_humidity, buffer);
		}
		else if(!strcmp(buffer, "2"))
		{
			fprintf(stdout, "\n최소 %s도를 변경합니다.\n", option == 1 ? "온" : "습");
			fprintf(stdout, ">> ");

			fgets(buffer, BUF_SIZE, stdin);
			buffer[strlen(buffer) - 1] = 0x00;

			if(option == 1)
				strcpy(alarm_info.min_temperature, buffer);
			else
				strcpy(alarm_info.min_humidity, buffer);
		}
		else if(!strcmp(buffer, "3"))
		{
			fprintf(stdout, "알림 주기를 변경합니다.\n");
			fprintf(stdout, ">> ");

			fgets(buffer, BUF_SIZE, stdin);
			buffer[strlen(buffer) - 1] = 0x00;

			memset(alarm_info.period, 0x00, sizeof(char) * SETTING_SIZE);
			strcpy(alarm_info.period, buffer);
		}
		else
		{
			fputs("다시 입력해주세요.\n", stdout);
		}

		fprintf(stdout, "변경되었습니다.\n\n");
	}
}

void display_change_alarm_info_period()
{
	char buffer[4] = {0,};

	fprintf(stdout, "\t\t---------------------\n");
	fprintf(stdout, "\t\t| 알림 주기 |  %s분  |\n", alarm_info.period);
	fprintf(stdout, "\t\t---------------------\n");
	fputc('\n', stdout);

	fprintf(stdout, "알림 주기를 변경합니다.\n");
	fprintf(stdout, ">> ");

	fgets(buffer, SETTING_SIZE, stdin);
	buffer[strlen(buffer) - 1] = 0x00;

	strcpy(alarm_info.period, buffer);

	fprintf(stdout, "변경되었습니다.\n\n");

	return ;
}
