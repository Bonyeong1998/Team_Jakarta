#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
	sigset_t set1, set2;

	/* fill set1 */
	sigfillset(&set1);
	/* set up set2 with just SIGINT */
	sigemptyset(&set2);
	sigaddset(&set2, SIGINT);
	printf("Critical region start.\n");
	sigprocmask(SIG_BLOCK, &set1, NULL); /* block */
	sleep(5);
	printf("Less critical region start.\n");
	sigprocmask(SIG_UNBLOCK, &set2, NULL); /* unblock SIGINT */
	sleep(5);
	printf("Non critical region start.\n");
	sigprocmask(SIG_UNBLOCK, &set1, NULL); /* unblock all */
	sleep(5);
	
	return 0;
}
