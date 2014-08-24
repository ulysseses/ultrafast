#include "gpu/BUtil.h"
#include <signal.h>  /* struct sigaction, sigfillset, sigaction */
#include <cstring>   /* memset */
// debug
#include <stdio.h>


bool quit = false;  // signal flag

void got_signal(int) { quit = true; }

int main( int argc, char **argv ) {
	/*
	initSharedMem();
	
	register exit callback
	atexit( exitCB );  // glut

	init GLUT and GL
	initGLUT( argc, argv );
	initGL();
	
	// get OpenGL info (need glInfo.cpp/h)
	glInfo gli;
	gli.getInfo();
	gli.printSelf();

	last GLUT call (loop)
	window will be shown and display callback is triggered by events
	NOTE: this call never returns main()
	glutMainLoop();  /* Start GLUT event-processing loop
	*/
	
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
	return EXIT_SUCCESS;
}


