#ifndef BUTIL_H
#define BUTIL_H

#include "gpu/OpenGL.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

bool initSharedMem();
void clearSharedMem();
// int initGLUT(int argc, char **argv);
bool initGL();
// void update();  // deprecated
void render();
// void idleCB();
// void exitCB();
void runMainLoop( int argc, char **argv );

#endif