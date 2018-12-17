//多线程 客户/服务器方式
//一个服务器线程 处理4个客户线程输入输出请求
#include "unp.h"

#define CLIENT_THREADS	4

#define REQ_READ		1
#define REQ_WRITE		2
#define REQ_QUIT		3

typedef struct request_tag
{
	struct request_tag	*next;
	int					operation;
	int					synchronous;
	int					done_flag;
	pthread_cond_t		done;
	char				prompt[32];
	char				text[128];
}request_t;

typedef struct tty_server_tag
{
	request_t		*first;
	request_t		*last;
	int				running;
	pthread_mutex_t	mutex;
	pthread_cond_t	request;
}tty_server_t;

tty_server_t tty_server = {
	NULL, NULL, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
};

int client_threads;
pthread_mutex_t	client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	clients_done = PTHREAD_COND_INITIALIZER;

void *tty_server_routine(void *arg)
{
	static pthread_mutex_t prompt_mutex = PTHREAD_MUTEX_INITIALIZER;
	request_t *request;
	int operation, len;
	int status;

	while (1)
	{
		status = pthread_mutex_lock(&tty_server.mutex);
		if (status != 0)
			err_abort(status, "Lock server mutex");

		while (tty_server.first == NULL)
		{
			status = pthread_cond_wait(&tty_server.request, &tty_server.mutex);
			if (status != 0)
				err_abort(status, "Wait for request");
		}

		request = tty_server.first;
		tty_server.first = request->next;
		if (tty_server.first == NULL)
			tty_server.last = NULL;

		status = pthread_mutex_unlock(&tty_server.mutex);
		if (status != 0)
			err_abort(status, "Unlock server mutex");

		operation = request->operation;
		switch (operation)
		{
		case REQ_QUIT:
			break;
		case REQ_READ:
			if (strlen(request->prompt) > 0)
				printf(request->prompt);
			if (fgets (request->text, 128, stdin) == NULL)
				request->text[0] = '\0';

			len = strlen(request->text);
			if (len > 0 && request->text[len - 1] == '\n')
				request->text[len - 1] = '\0';
			break;
		case REQ_WRITE:
			puts(request->text);
			break;
		default:
			break;
		}

		if (request->synchronous)
		{
			status = pthread_mutex_lock(&tty_server.mutex);
			if (status != 0)
				err_abort(status, "Lock server mutex");
			request->done_flag = 1;
			status = pthread_cond_signal(&request->done);
			if (status != 0)
				err_abort(status, "Unlock server mutex");
			status = pthread_mutex_unlock(&tty_server.mutex);
			if (status != 0)
				err_abort(status, "Unlock server mutex");
		}
		else
			free(request);
		if (operation == REQ_QUIT)
			break;
	}
	return NULL;
}

void tty_server_request(int operation, int sync, const char *prompt, char *string)
{
	request_t *request;
	int status;

	status = pthread_mutex_lock(&tty_server.mutex);
	if (status != 0)
		err_abort(status, "Lock server mutex");
	if (!tty_server.running)
	{
		pthread_t thread;
		pthread_attr_t detached_attr;

		status = pthread_attr_init(&detached_attr);
		if (status != 0)
			err_abort(status, "Init attributes object");
		status = pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
		if (status != 0)
			err_abort(status, "Set detach state");

		tty_server.running = 1;
		status = pthread_create(&thread, &detached_attr, tty_server_routine, NULL);
		if (status != 0)
			err_abort(status, "Create server");
		pthread_attr_destroy(&detached_attr);
	}

	request = (request_t*)malloc(sizeof(request_t));
	if (request == NULL)
		errno_abort("Allocate request");

	request->next = NULL;
	request->operation = operation;
	request->synchronous = sync;
	if (sync)
	{
		request->done_flag = 0;
		status = pthread_cond_init(&request->done, NULL);
		if (status != 0)
			err_abort(status, "Init request condition");
	}

	if (prompt != NULL)
		strncpy(request->prompt, prompt, 32);
	else
		request->prompt[0] = '\0';
	if (operation == REQ_WRITE && string != NULL)
		strncpy(request->text, string, 128);
	else
		request->text[0] = '\0';

	if (tty_server.first == NULL)
	{
		tty_server.first = request;
		tty_server.last = request;
	}
	else
	{
		(tty_server.last)->next = request;
		tty_server.last = request;
	}

	status = pthread_cond_signal(&tty_server.request);
	if (status != 0)
		err_abort(status, "Wake server");

	if (sync)
	{
		while (!request->done_flag)
		{
			status = pthread_cond_wait(&request->done, &tty_server.mutex);
			if (status != 0)
				err_abort(status, "Wait for sync request");
		}
		if (operation == REQ_READ)
		{
			if (strlen(request->text) > 0)
				strcpy(string, request->text);
			else
				string[0] = '\0';
		}
		status = pthread_cond_destroy(&request->done);
		if (status != 0)
			err_abort(status, "Destory request condition");
		free(request);
	}
	status = pthread_mutex_unlock(&tty_server.mutex);
	if (status != 0)
		err_abort(status, "Unlock mutex");
}

void *client_routine(void *arg)
{
	int my_number = (int)arg, loops;
	char prompt[32];
	char string[128], formatted[128];
	int status;

	sprintf(prompt, "Client %d> ", my_number);
	while (1)
	{
		tty_server_request(REQ_READ, 1, prompt, string);
		if (strlen(string) == 0)
			break;
		for (loops = 0; loops < 4; loops++)
		{
			sprintf(formatted, "(%d#%d) %s", my_number, loops, string);
			tty_server_request(REQ_WRITE, 0, NULL, formatted);
			sleep(1);
		}
	}
	status = pthread_mutex_lock(&client_mutex);
	if (status != 0)
		err_abort(status, "Lock client mutex");
	client_threads--;
	if (client_threads <= 0)
	{
		status = pthread_cond_signal(&clients_done);
		if (status != 0)
			err_abort(status, "Signal client done");
	}
	status = pthread_mutex_unlock(&client_mutex);
	if (status != 0)
		err_abort(status, "Unlock client mutex");
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t thread;
	int count;
	int status;

	client_threads = CLIENT_THREADS;
	for (count = 0; count < client_threads; count++)
	{
		status = pthread_create(&thread, NULL, client_routine, (void*)count);
		if (status != 0)
			err_abort(status, "Create client thread");
	}

	status = pthread_mutex_lock(&client_mutex);
	if (status != 0)
		err_abort(status, "Lock client mutex");
	while (client_threads > 0)
	{
		status = pthread_cond_wait(&clients_done, &client_mutex);
		if (status != 0)
			err_abort(status, "Wait for clients to finish");
	}
	status = pthread_mutex_unlock(&client_mutex);
	if (status != 0)
		err_abort(status, "Unlock client mutex");
	printf("All clients done\n");
	tty_server_request(REQ_QUIT, 1, NULL, NULL);
	return 0;
}
