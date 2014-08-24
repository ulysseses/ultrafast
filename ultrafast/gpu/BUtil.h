#ifndef BUTIL_H
#define BUTIL_H


bool initSharedMem();
void clearSharedMem();
bool initGL();
void render();

class BContext {
public:
	BContext();
	~BContext();
};


#endif