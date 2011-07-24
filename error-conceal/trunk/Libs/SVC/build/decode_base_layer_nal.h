#ifndef _DECODE_BASE_LAYER_NAL_H_
#define _DECODE_BASE_LAYER_NAL_H_

#include "type.h"
#include "svc_type.h"
#include "decoder_context.h"


void decode_base_layer_nal(NAL* pNal,decoder_context* pdecoder_context,
						   thread_context* pthread_context,display_context* pdisplay_con);

void decode_base_layer_idr_nal(NAL* pNal,decoder_context* pdecoder_context,
							   thread_context* pthread_context,display_context* pdisplay_con);


#endif