#include "UF_ZMQ.h"
#include <stdio.h>	// perror, printf
#include <string.h> // memcpy
#include <stdlib.h> // exit
#include "sizes.h"
// debugging
#include <errno.h>
#include <assert.h>


#define RAWSIZE (IMAGE_WIDTH * IMAGE_HEIGHT * NUM_CHANNEL)
#define IMGSIZE (SCREEN_WIDTH * SCREEN_HEIGHT * NUM_CHANNEL)

static byte rawdata[RAWSIZE];
static byte imgdata[IMGSIZE];

static char *servername;

int main (int argc, char **argv) {
	if (argc < 3) {
		printf("syntax: client server file\n");
		return 0;
	}
	FILE *fp;
	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		perror("client: failed to read rawdata file");
		printf("errno: %d\n", errno);
		exit(1);
	}
	size_t bytes_read = fread(rawdata, sizeof(byte), RAWSIZE, fp);
	if (bytes_read != RAWSIZE) {
		perror("client: reading error");
		printf("errno: %d\n", errno);
		exit(1);  // ?
	}
	// whole file is now loaded in mem buf
	fclose(fp);
	printf("client: yay, file loaded in mem buf\n");
	assert(rawdata[0] == 120);
	assert(rawdata[1] == 123);

	zctx_t *ctx = zctx_new();
	// socket
	void *server = zsocket_new(ctx, ZMQ_DEALER);
	zsocket_connect(server, "ipc://%s.ipc", servername);

	zframe_t *frame = zframe_new(rawdata, RAWSIZE*sizeof(byte));
	int blah = zframe_send(&frame, server, 0);
	if (blah == -1) {
		printf("client: sending to server error: %s\n", zmq_strerror(errno));

	}
	else {
		printf("client: successfully sent to server\n");
	}
	while (true) {
		zmq_pollitem_t pollset[] = {
			{ server, 0, ZMQ_POLLIN, 0 }
		};
		printf("client: im working..\n");
		int rc = zmq_poll(pollset, 1, -1);  // indefinite wait
		if (rc == -1) break;
		if (pollset[0].revents & ZMQ_POLLIN) {
			frame = zframe_recv(server);
			if (!frame) break;
			// frame is the image
			memcpy((void*)imgdata, (void*)zframe_data(frame), IMGSIZE);
			printf("copy from frame buffer to static local buffer worked!\n");
			break;
		}
	}  /* while (true) */
	zctx_destroy(&ctx);
	return 0;
}