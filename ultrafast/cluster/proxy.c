#include "UF_ZMQ.h"


int main (int argc, char * argv[]) {
	zctx_t *ctx = zctx_new();
	
	/* Sockets */
	void *frontend	= zsocket_new(ctx, ZMQ_ROUTER);
	void *backend	= zsocket_new(ctx, ZMQ_ROUTER);
	zsocket_bind(frontend, "ipc://proxy-fe.ipc");
	zsocket_bind(backend, "ipc://proxy-be.ipc");
	
	/* Backend Load-balance Queue */
	int capacity		= 0;
	zlist_t *workers	= zlist_new();
	
	/* Main loop */
	zmsg_t *msg = NULL;
	while (true) {
		// zmq polling boilerplate
		zmq_pollitem_t pollset[] = {
			{ frontend, 0, ZMQ_POLLIN, 0 },
			{ backend, 0, ZMQ_POLLIN, 0 }
		};
		int rc = zmq_poll(pollset, 2,
			capacity ? 1000 * ZMQ_POLL_MSEC : -1);
		if (rc == -1) break;  // interrupted
		
		if (pollset[0].revents & ZMQ_POLLIN) {
			msg = zmsg_recv(frontend);
			if (!msg) break;
			zframe_t *frame = zframe_new_empty();
			zmsg_prepend(msg, &frame);
			frame = (zframe_t*) zlist_pop(workers);
			zmsg_prepend(msg, (zframe_t **) &frame);
			zmsg_send(&msg, backend);
			capacity--;
		}
		else if (pollset[1].revents & ZMQ_POLLIN) {
			msg = zmsg_recv(backend);
			if (!msg) break;
			zframe_t *worker_id = zmsg_pop(msg);
			// distinguish msg from DECODER or ENCODER
			if (memcmp(zframe_data(zmsg_last(msg)), WORKER_READY, 1)
					== 0)
				zmsg_destroy(&msg);
			else
				zmsg_send(&msg, frontend);
				
			zlist_append(workers, worker_id);
			capacity++;
		}
		
	}  /* while (true) */
	
	/* Destruction */
	while (zlist_size (workers)) {
		zframe_t *frame = (zframe_t *) zlist_pop(workers);
		zframe_destroy(&frame);
	}
	zlist_destroy(&workers);
	zctx_destroy(&ctx);
	return EXIT_SUCCESS;
}  /* main */































