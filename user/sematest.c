#include "kernel/types.h"
#include "user/user.h"

// 0 inti
// 1 downsema
// 2 upsema

int 
main()
{
	int i, pid;

	// initialize the semaphore
	sematest(0);

	for (i = 0; i < 10; i++) {
		pid = fork();

		if (!pid) // child threads, (!pid) == (!0) == true
			break;
	}
	
	if (pid) { // parent thread
//		sleep(300);
		for (i = 0; i < 10; i++) 
			wait(0);
		
		sematest(1);
		printf("Final %d\n", sematest(2));
	} 
	else { // child thread
		printf("%d Down : %d\n", i, sematest(1));
		sleep(100);	
		printf("%d   Up : %d\n", i, sematest(2));
	}
	
	exit(0);
}

