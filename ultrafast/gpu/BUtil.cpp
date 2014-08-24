#include "gpu/BUtil.h"
#include "gpu/BTexture.h"
#include <stdio.h>  /* debugging features */


BTexture *texObj;


bool initSharedMem() {
	// disable zmq's automatic signal handling
	zsys_handler_set(NULL);
	
	texObj = new BTexture(IMAGE_WIDTH, IMAGE_HEIGHT,
		SCREEN_WIDTH, SCREEN_HEIGHT, "proxy-be", "proxy-be");
	printf("texObj created!\n");
	return true;
}

void clearSharedMem() {
	// clean up texture on GPU
	delete texObj;
	printf("texObj cleaned!\n");
}


bool initGL() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 0.f);  // background color
	
	// Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 1.0 );
	
	// Initialize ModelView Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	// check for gl error
	GLenum error = glGetError();
	if ( error != GL_NO_ERROR ) {
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );  // deprecate this?
		return false;
	}
	
	return true;
}

void render() {
	glClear( GL_COLOR_BUFFER_BIT );
	//GLfloat x = ( SCREEN_WIDTH - texObj.imageWidth() ) / 2.f;
	//GLfloat y = ( SCREEN_HEIGHT - texObj.imageHeight() ) / 2.f;
	//texObj->render( x, y );
	texObj->render();
	//glutSwapBuffers();  // show in viewport
}



BContext::BContext()  {  initSharedMem(); }
BContext::~BContext() { clearSharedMem(); }


