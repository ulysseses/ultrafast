#ifndef BTEXTURE_H
#define BTEXTURE_H


#include "UF_ZMQ.h"
#include "gpu/OpenGL.h"
#include "gpu/VertexData2D.h"
#include <string>

class BTexture {
private:
	/* ZMQ */
	void *frontend;
	void *backend;
	zctx_t *ctx;
	zmsg_t *msg;
	zframe_t *frame;
	
	/* OpenGL */
	GLuint imageWidth, imageHeight;
	size_t imageSize;
	GLuint textureWidth, textureHeight;
	size_t textureSize;
	GLuint screenWidth, screenHeight;
	size_t screenSize;
	
	int index;
	int nextIndex;
	GLuint texID, tboID, vtcboID, iboID, pboIDs[2];
	GLubyte *tData;
	VertexData2D *vData;
	GLushort *iData;
	GLubyte *pixels;
	
	GLuint powerOfTwo( GLuint num );
	
public:
	BTexture( GLuint iwidth, GLuint iheight,
		GLuint swidth, GLuint sheight,
		std::string fe_path, std::string be_path );
	~BTexture();
	void freeTexture();
	//void render( GLfloat x, GLfloat y);
	void render();
	void queuePop();
	void queuePush();
};



#endif

