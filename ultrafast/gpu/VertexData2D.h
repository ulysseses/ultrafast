#ifndef VERTEXDATA2D_H
#define VERTEXDATA2D_H

#include "gpu/OpenGL.h"


struct VertexPos2D {
	GLfloat x;
	GLfloat y;
};

struct TexCoord {
	GLfloat s;
	GLfloat t;
};

struct VertexData2D {
	VertexPos2D position;
	TexCoord texCoord;
};


#endif











