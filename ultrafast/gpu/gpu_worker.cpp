#include "gpu/BUtil.h"
#include <signal.h>	/* struct sigaction, sigfillset, sigaction */
#include <string.h>	/* memset */
#include <stdio.h>	/* debugging */


bool quit = false;  // signal flag

void got_signal(int) { quit = true; }

int main( int argc, char **argv ) {
	
	// override SIGINT POSIX behavior
	// NOTE: doesn't interact well with zctx :(
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = got_signal;
	sigfillset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);  // SIGINT or SIGSTOP

	initGL();
	BContext bc;  // RAII
	while (true) {
		render();
		if (quit) {
			printf("\ninterrupted...\n");
			break;  // exit normally after SIGINT
		}
	}
	printf("\ngpu_worker exit...\n");
	return 0;
}


