// gcc -o testgl testgl.cpp -lGL -lGLU -lX11

#define GL_GLEXT_PROTOTYPES 1  // for GL/glext.h
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>

/* You need a implementation specific OpenGL Context
in order to use OpenGL API */
#include <X11/X.h>     /* Window, None */
#include <X11/Xlib.h>  /* Display, XOpenDisplay */
#include <GL/glx.h>    /* XVisualInfo (through X11/Xutil.h), GLXContext,
	GLXFBConfig, GLXPbuffer */


int main (int argc, char **argv) {
	/* https://sidvind.com/wiki/Opengl/windowless */
	// Display *dpy;
	// Window root;
	// GLint attr[] = { None };
	// XVisualInfo *vi;
	// GLXContext glc;

	// /* open display */
	// if (!(dpy = XOpenDisplay(NULL))) {
	// 	fprintf(stderr, "cannot connect to X server\n\n");
	// 	exit(1);
	// }

	// /* get root window */
	// root = DefaultRootWindow(dpy);

	// /* get visual matching attr */
	// if (!(vi = glXChooseVisual(dpy, 0, attr))) {
	// 	fprintf(stderr, "no appropriate visual found\n\n");
	// 	exit(1);
	// }

	static int visual_attribs[] = {
		None
	};

	int context_attribs[] = {
		None
	};

	Display *dpy = XOpenDisplay(0);
	int fbcount = 0;
	GLXFBConfig *fbc = NULL;
	GLXContext ctx;
	GLXPbuffer pbuf;

	/* open display */
	if (!(dpy = XOpenDisplay(0))) {
		fprintf(stderr, "Failed to open display\n");
		exit(1);
	}

	/* get framebuffer configs, any is usuable (might want to add proper attribs) */
	if (!(fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), visual_attribs, &fbcount))) {
		fprintf(stderr, "Failed to get FBConfig\n");
		exit(1);
	}




	// target code
	GLuint arrID[3] = {0, 0, 0};
	glGenBuffers(3, arrID);
	printf("arrID: %d\n", arrID[1]);
	return 0;
}

