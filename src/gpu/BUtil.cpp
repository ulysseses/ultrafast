#include "gpu/OpenGL.h"
#include "gpu/BUtil.h"
#include "gpu/BTexture.h"
#include "sizes.h"


extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

BTexture *texObj;


bool initSharedMem() {
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;
	texObj = new BTexture(IMAGE_WIDTH, IMAGE_HEIGHT,
		SCREEN_WIDTH, SCREEN_HEIGHT, "gpu");
	return true;
}

void clearSharedMem() {
	// clean up texture on GPU
	delete texObj;
}

int initGLUT( int argc, char **argv ) {
	// GLUT stuff for windowing
	// initialized before any other GLUT routines
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_LUMINANCE );  // display mode
	glutInitWindowSize( screenWidth, screenHeight );
	glutInitWindowPosition( 100, 100 );
	int handle = glutCreateWindow(argv[0]);  // param is window title
	
	// finally, create a window w/ OpenGL context
	// window not displayed until glutMainLoop() is called
	glutDisplayFunc( render );
	glutIdleFunc( idleCB );
	
	return handle;
}

bool initGL() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.f, 0.f, 0.f, 0.f);  // background color
	
	// Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, screenWidth, screenHeight, 0.0, 1.0, -1.0 );
	
	// Initialize ModelView Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	// check for gl error
	GLenum error = glGetError();
	if ( error != GL_NO_ERROR ) {
		std::printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );  // deprecate this!
		return false;
	}
	
	return true;
}

void render() {
	glClear( GL_COLOR_BUFFER_BIT );
	//GLfloat x = ( screenWidth - texObj.imageWidth() ) / 2.f;
	//GLfloat y = ( screenHeight - texObj.imageHeight() ) / 2.f;
	//texObj.render( x, y );
	texObj.render();
	//glutSwapBuffers();  // show in viewport
}

void idleCB() {
	glutPostRedisplay();
}

void exitCB() {
	clearSharedMem();
}




