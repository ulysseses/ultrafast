#include "gpu/BUtil.h"



int main( int argc, char **argv ) {
	//initSharedMem();
	
	// register exit callback
	// atexit( exitCB );  // glut

	// init GLUT and GL
	// initGLUT( argc, argv );
	initGL();
	
	// // get OpenGL info (need glInfo.cpp/h)
	// glInfo gli;
	// gli.getInfo();
	// gli.printSelf();

	// last GLUT call (loop)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never returns main()
	// glutMainLoop();  /* Start GLUT event-processing loop */

	return EXIT_SUCCESS;
}


