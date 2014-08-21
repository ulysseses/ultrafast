#ifndef WORKER_H
#define WORKER_H

#include "UF_ZMQ.h"

typedef byte* (* decorableFnType)(byte*);

extern byte* decoder (byte *unprocessed_data, size_t size);
extern byte* encoder (byte *unprocessed_data, size_t size);


void zworker_boiler(decorableFnType worker_fn,
	char *fe_path, char *be_path);

#endif