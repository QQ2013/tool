#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
void main()
{
	setbuf(stdout,NULL);
	signal(SIGCHLD,SIG_IGN);
    int i = 0;
	pid_t pid=0;
	printf("jh");

	for (i=0;i<255;i++)
	{
		pid = fork();
	    if (pid==(pid_t)0)
		{
			return ;
		}
	}
	pause();
}
