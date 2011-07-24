#ifndef _LOG_H_
#define _LOG_H_

#include "type.h"
#include "svc_type.h"
#include "decoder_context.h"
void log_init();
void log_flush();
void log_exit();
void logStr(const char* format,...);
void log_memory_state(NAL *pcNal,MMO *pcMmo);
void log_slice_info(NAL *pcNal,decoder_context* pdecoder_context,
						   thread_context* pthread_context);

#endif