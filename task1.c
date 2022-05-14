#include <sys/types.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>


void getTime(char* caller){
	struct timeval tv;
	
	if (gettimeofday(&tv, NULL) == -1)
		perror("Can not get current time\n");
	else {
	
		int mls = tv.tv_usec / 1000;
		int ss = tv.tv_sec % 60;
		int mm = (tv.tv_sec / 60) % 60;
		int hh = (tv.tv_sec / 3600 + 3) % 24;
	
		printf("%s PID %d PPID %d Time: %02d:%02d:%02d:%03d\n\n", caller, getpid(), getppid(), hh, mm, ss, mls);	
	}
}

void waitChild(pid_t pid) {
	if (waitpid(pid, NULL, 0) == -1)
		perror("wait pid failure\n");
}

void main(void){
	pid_t child1, child2;
	
	if ((child1 = fork()) > 0 && (child2 = fork()) > 0) {
		
		getTime("Parent");
		
		if (system("ps -x") == -1)
			perror("Can not report a snapshot of the current processes");
		
		waitChild(child1);
		waitChild(child2);
	}
	
	if (child1 < 0)
    	{
        	perror("Child 1 process could not be created\n");
    	}
    	else if (child1 == 0)
    	{
        	getTime("Child 1");
    	}
    	else if (child2 < 0)
    	{
        	perror("Child 2 process could not be created\n");
		waitChild(child1);
    	}
    	else if (child2 == 0)
    	{
        	getTime("Child 2");
    	}
	
	return;
}
