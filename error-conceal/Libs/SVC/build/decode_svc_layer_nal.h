#ifndef _DECODE_SVC_LAYER_NAL_H_
#define _DECODE_SVC_LAYER_NAL_H_

#include "decoder_context.h"

void decode_svc_layer_nal(NAL *pNal,decoder_context* pdecoder_context,thread_context *pthread_context,
						  display_context *pdisplay_context);

void decode_prefix_nal(NAL *pNal,decoder_context *pdeocder_context,
					   thread_context *pthread_context);

#endif