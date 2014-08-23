/* Request Dealer Sandwich Worker */
#include "codec/worker.h"

/*
inline byte* decoder(byte *unprocessed_data, size_t size) {
	// identity
	byte *processed_data = (byte*) malloc(size);
	memcpy(processed_data, unprocessed_data, size);
	//free(unprocessed_data);
	return processed_data;
}

inline byte* encoder(byte *unprocessed_data, size_t size) {
	// identity
	byte *processed_data = (byte*) malloc(size);
	memcpy(processed_data, unprocessed_data, size);
	//free(unprocessed_data);
	return processed_data;
}
*/

void zworker_boiler(decorableFnType worker_fn,
	char *fe_path, char *be_path) {
	zctx_t *ctx = zctx_new();
	
	/* Sockets */
	void *frontend	= zsocket_new(ctx, ZMQ_REQ);
	void *backend	= zsocket_new(ctx, ZMQ_DEALER);
	zsocket_connect(frontend, "ipc://%s-fe.ipc", fe_path);
	zsocket_connect(backend, "ipc://%s-be.ipc", be_path);
	
	/* Main worker loop */
	zframe_t *frame = zframe_new(WORKER_READY, 1);
	zframe_send(&frame, frontend, 0);
	while (true) {
		zmsg_t *msg = zmsg_recv(frontend);
		if (!msg) break;
		frame = zmsg_last(msg);
		size_t size = zframe_size(frame);
		byte *data = zframe_data(frame);
		byte *processed_data = worker_fn(data, &size);  // modify size?
		zframe_reset(frame, processed_data, size);
		zmsg_send(&msg, backend);
	}  /* while (true) */
	
	zctx_destroy(&ctx);
}





































