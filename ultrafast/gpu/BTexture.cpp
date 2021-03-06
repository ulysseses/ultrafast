#include "gpu/BTexture.h"
#include "sizes.h"
#include <string>	 /* c++ string dependence */
#include <string.h>  /* memset, memcpy */
#include <assert.h>  /* assert */
#include <errno.h>   /* debugging */

#define PI 3.14159265359

static const float SECTOR_RAD	= 1.3184;
static const float NUM_BEAMS	= 128;  // Note: FLOAT not INT
static const float PROBE_R_N	= 0.2f;
static const int PROBE_R		= SCREEN_HEIGHT / 5;
static const int SCAN_D_N		= 1.f - PROBE_R_N;
static const int SCAN_D			= SCREEN_HEIGHT - PROBE_R;

static const float X0_N			= 0.5f;
static const int X0				= SCREEN_WIDTH / 2;
static const float Y0_N			= 0.f;
static const int Y0				= SCREEN_HEIGHT * 0;
static const float ANGLE0		= PI + 0.5*(PI - SECTOR_RAD);

static const int NUM_VERTICES	= 2 * NUM_BEAMS;
static const int NUM_ELEMENTS	= (NUM_BEAMS - 1) * 6;



BTexture::BTexture( GLuint iwidth, GLuint iheight,
	GLuint swidth, GLuint sheight,
	std::string fe_path, std::string be_path ) {
	/* ZMQ */
	ctx = zctx_new();
	frontend	= zsocket_new(ctx, ZMQ_REQ);
	backend		= zsocket_new(ctx, ZMQ_DEALER);
	zsocket_connect(frontend, "ipc://%s.ipc", fe_path.c_str());
	zsocket_connect(backend, "ipc://%s.ipc", be_path.c_str());
	
	zframe_t *frame = zframe_new(WORKER_READY, 1);
	zframe_send(&frame, frontend, 0);
	
	/* OpenGL */
	imageWidth		= iwidth;
	imageHeight		= iheight;
	imageSize		= iwidth * iheight;
	textureWidth	= powerOfTwo(iwidth);
	textureHeight	= powerOfTwo(iheight);
	textureSize		= textureWidth * textureHeight;
	screenWidth		= swidth;
	screenHeight	= sheight;
	screenSize		= swidth * sheight;
	
	tData			= NULL;
	vData			= NULL;
	iData			= NULL;
	pixels			= NULL;
	tboID			= 0;
	texID			= 0;
	vtcboID			= 0;
	iboID			= 0;
	index			= 0;
	nextIndex		= 0;

	// vData
	vData = new VertexData2D[NUM_VERTICES];
	float angle;
	int i;
	for (angle=ANGLE0, i=0; i<NUM_BEAMS/2;
		angle+=SECTOR_RAD/NUM_BEAMS, ++i) {
		vData[i].position.x = X0_N + PROBE_R_N*cos(angle);
		vData[i].position.y = X0_N + PROBE_R_N*sin(angle);
		vData[i].texCoord.s = i / NUM_BEAMS;
		vData[i].texCoord.t = 0.f;
		vData[i+((int)NUM_BEAMS/2)].position.x = X0_N + SCAN_D_N*cos(angle);
		vData[i+((int)NUM_BEAMS/2)].texCoord.s = i / NUM_BEAMS;
		vData[i+((int)NUM_BEAMS/2)].position.y = Y0_N + SCAN_D_N*sin(angle);
		vData[i+((int)NUM_BEAMS/2)].texCoord.t = 1.f;
	}
	// iData
	iData = new GLushort[NUM_ELEMENTS];
	const int bot_domain = NUM_ELEMENTS / 2;
	int ind = 0;
	for (size_t i=0; i < NUM_BEAMS-1; ++i) {
		iData[ind++]	= i;
		iData[ind++]	= i + bot_domain;
		iData[ind++]	= i + bot_domain + 1;
		iData[ind++]	= i;
		iData[ind++]	= i + 1;
		iData[ind++]	= i + bot_domain + 1;
	}
	
	// Texture & Texture Buffer Object
	glGenBuffers(1, &tboID);
	printf("tboID is buffer? %d\n", glIsBuffer(tboID));
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	GLubyte zeros[textureSize];
	memset(zeros, 0, textureSize * sizeof(GLubyte));
	glBufferData(GL_TEXTURE_BUFFER, textureSize * sizeof(GLubyte),
		zeros, GL_STREAM_COPY);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_BUFFER, texID);
	glTexParameteri(GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_BUFFER, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Vertex, TexCoord, & VertexData2D array buffer object
	glGenBuffers(1, &vtcboID);
	glBindBuffer(GL_ARRAY_BUFFER, vtcboID);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(VertexData2D),
		vData, GL_STATIC_DRAW);
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_ELEMENTS * sizeof(GLushort),
		iData, GL_STATIC_DRAW);  // maybe don't need...
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(VertexData2D),
		(GLvoid*)offsetof(VertexData2D, position));
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData2D),
		(GLvoid*)offsetof(VertexData2D, texCoord));
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// Pixel Pack Buffer Object
	glGenBuffers(2, pboIDs);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIDs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER,
		screenSize * sizeof(GLubyte), NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIDs[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER,
		screenSize * sizeof(GLubyte), NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	// Set target framebuffer for pixel reading
	glReadBuffer(GL_FRONT);
}

BTexture::~BTexture() {
	freeTexture();
}

void BTexture::freeTexture() {
	/* ZMQ */
	if (frame) zframe_destroy(&frame);
	if (msg) zmsg_destroy(&msg);
	zctx_destroy(&ctx);
	
	/* OpenGL */
	if (texID) {
		glDeleteTextures(1, &texID);
		texID = 0;
	}
	if (tboID)		{ glDeleteBuffers(1, &tboID); tboID = 0; }
	if (vtcboID)	{ glDeleteBuffers(1, &vtcboID); vtcboID = 0; }
	if (iboID)		{ glDeleteBuffers(1, &iboID); iboID = 0; }
	if (pboIDs[0] & pboIDs[1]) {
		glDeleteBuffers(2, pboIDs);
		memset(pboIDs, 0, 2 * sizeof(GLuint));
	}
	
	if (tData) delete[] tData;
	if (vData) delete[] vData;
	if (iData) delete[] iData;
	if (pixels) delete[] pixels;
	
	imageWidth		= 0;
	imageHeight		= 0;
	imageSize		= 0;
	textureWidth	= 0;
	textureHeight	= 0;
	textureSize		= 0;
	screenWidth		= 0;
	screenHeight	= 0;
	screenSize		= 0;
	
}

//void BTexture::render( GLfloat x, GLfloat y ) {
void BTexture::render() {
	if (texID) {
		// remove previous ModelView transform
		glLoadIdentity();
		//glTranslatef(x, y, 0.f);
		
		queuePop();
		glDrawElements(GL_TRIANGLES, NUM_ELEMENTS, GL_UNSIGNED_BYTE, NULL);
		queuePush();
		
	}  /* if (textureID) */
}

void BTexture::queuePop() {
	msg = zmsg_recv(frontend);
	if (!msg) {
		printf("queuPop error:%s\n", zmq_strerror(errno));
		printf("exitiing...\n");
		exit(EXIT_FAILURE);
	}
	printf("queuePop: received data!\n");
	frame = zmsg_last(msg);
	GLubyte *p_tData = (GLubyte*) glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_WRITE);
	assert (memcpy((void*)p_tData, (void*)zframe_data(frame),
		textureSize * sizeof(GLubyte)) != NULL);
	assert (glUnmapBuffer(GL_TEXTURE_BUFFER) == GL_TRUE);
}

void BTexture::queuePush() {
	// index's pbo does (asnyc) DMA memory transfer
	index = (index + 1) % 2;
	nextIndex = (index + 1) % 2;
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIDs[index]);
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RED,
		GL_UNSIGNED_BYTE, NULL);
	// nextIndex's pbo pins to system memory for ZMQ msg sending
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIDs[nextIndex]);
	GLubyte *p_pixels=(GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	zframe_reset(frame, p_pixels, textureSize * sizeof(GLubyte));
	zmsg_send(&msg, backend);
	assert (glUnmapBuffer(GL_PIXEL_PACK_BUFFER) == GL_TRUE);
}


GLuint BTexture::powerOfTwo( GLuint num ) {
	if (num != 0) {
		num--;
		num |= (num >> 1);  // OR first 2 bits
		num |= (num >> 2);  // OR next 2 bits
		num |= (num >> 4);  // OR next 4 bits
		num |= (num >> 8);  // OR next 8 bits
		num |= (num >> 16); // OR next 16 bits
		num++;
	}
	return num;
}




