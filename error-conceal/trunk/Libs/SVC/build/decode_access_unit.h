#ifndef _DECODE_ACCESS_UNIT_H_
#define _DECODE_ACCESS_UNIT_H_

#include "decoder_context.h"

void after_decode_au(decoder_context *pdecoder_context,display_context* pdisplay_context,thread_context* pthread_context);

void before_decoder_au(decoder_context* pdecoder_context,display_context* pdisplay_context);
#endif