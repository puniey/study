#include "unp.h"

void *thread_routine(void *arg)
{
	return arg;
}

int main(int argc, char **argv)
{
	pthread_t	tpid;
	void *thread_result;
	int status;

	status = pthread_create(&tpid, NULL, thread_routine, NULL);
	if (status != 0)
		err_abort(status, "create thread");

	status = pthread_join(tpid, &thread_result);
	if (status != 0)
		err_abort(status, "join thread");
	if (thread_result == NULL)
		return 0;
	else
		return 1;
}
