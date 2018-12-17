#include "unp.h"

static int tBegin;
static void sig_alarm(int signo)
{
	int tNow = time(NULL);
	printf("time elapse %ds\n", (tNow - tBegin));
}

int main(int argc, char **argv)
{
	int nret;
	tBegin = time(NULL);
	if (Signal(SIGALRM, sig_alarm) == SIG_ERR)
	{
		perror("sinal error");
		exit(0);
	}
	nret = alarm(5);
    printf("nret:%d\n", nret);
	nret = alarm(0);
    printf("nret:%d\n", nret);
	while (1)
		pause();
	exit(0);
}
